import json
import time
import glob
import os
import subprocess
import pathlib
import threading
import shutil
from datetime import datetime, timezone, timedelta
from os.path import splitext, basename, exists, dirname
from collections import OrderedDict
from flask import Flask, request, render_template, abort, send_from_directory, make_response
from werkzeug.utils import secure_filename

static_path = pathlib.Path(__file__).resolve().parent / 'static'
repo_path = pathlib.Path(__file__).resolve().parent.parent
app = Flask(__name__, static_folder = str(static_path), static_url_path='')
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0
JST = timezone(timedelta(hours=+9), 'JST')
PORTAL_ENV = os.environ.get("PORTAL_ENV")
MY_TEAM_ID = '0017'

def make_url(relpath):
    if PORTAL_ENV == 'ec2':
        return "http://18.179.226.203/" + relpath
    return relpath

def visualizer_url(prob_id, solution_path):
    return 'http://18.179.226.203/shohei/visualizer/bin/index.html#{"model":"' + prob_id + '","file":"' + solution_path + '"}'

@app.after_request
def add_header(response):
    if 'Expires' in response.headers:
        del response.headers['Expires']
    response.headers['Cache-Control'] = 'no-store'
    return response

@app.route('/problemsF/<name>', methods=['GET'])
def get_problems_F(name):
    return send_from_directory(repo_path / 'problemsF', name)

def collect_probs():
    return [os.path.relpath(path, str(repo_path))
            for path in glob.glob(str(repo_path / 'problemsF') + '/*.r', recursive=True)]

def collect_nbts(exclude_ais=[]):
    nbts = []

    for path in glob.glob(str(repo_path / 'out') + '/**/*.nbt.gz', recursive=True):

        exclude = False
        for exclude_ai in exclude_ais:
            if exclude_ai in path:
                exclude = True
                break
        if exclude:
            continue

        prefix = path.split('.')[0]
        prob_id = basename(path).split('.')[0]
        ai_name = basename(dirname(path))
        prob_src_path = str(repo_path / 'problemsF' / prob_id) + '_src.mdl'
        prob_tgt_path = str(repo_path / 'problemsF' / prob_id) + '_tgt.mdl'
        validate_path = prefix + '.validate'
        javalidate_path = prefix + '.javalidate'
        sc6_path = prefix + '.nbt.sc6'
        r = 0
        cost = 0
        sc6_cost = 0
        valid = None
        javalid = None
        step = 0

        if not exists(prob_src_path):
            prob_src_path = None
        if not exists(prob_tgt_path):
            prob_tgt_path = None

        if prob_src_path:
            with open(prob_src_path, 'rb') as f:
                r = int.from_bytes(f.read(1), 'little')
        else:
            with open(prob_tgt_path, 'rb') as f:
                r = int.from_bytes(f.read(1), 'little')

        if exists(validate_path):
            with open(validate_path, 'r') as f:
                for s in f:
                    if s.startswith('Failure'):
                        valid = 0
                    if s.startswith('Success'):
                        valid = 1
                    if s.startswith('Time'):
                        step = int(s.split(' ')[-1].strip())
                    if s.startswith('Energy'):
                        cost = int(s.split(' ')[-1].strip())

        if exists(javalidate_path):
            with open(javalidate_path, 'r') as f:
                x = json.loads(f.read())
                if x['result'] == 'success':
                    javalid = x['energy']
                else:
                    javalid = 0

        if exists(sc6_path):
            with open(sc6_path, 'r') as f:
                sc6_cost = int(f.read().strip())

        nbts.append({
            "path" : path,
            "step" : step,
            "prefix" : prefix,
            "prob_id" : prob_id,
            "ai_name" : ai_name,
            "prob_src_path" : prob_src_path,
            "prob_tgt_path" : prob_tgt_path,
            "validate_path" : validate_path,
            "javalidate_path" : javalidate_path,
            "r" : r,
            "cost" : cost,
            "sc6_cost" : sc6_cost,
            "valid" : valid,
            "javalid" : javalid,
        })

    return nbts

def by_prob_map(nbts):
    by_prob = {}
    for nbt in nbts:
        prob_id = nbt['prob_id']
        if prob_id not in by_prob:
            by_prob[prob_id] = []
        by_prob[prob_id].append(nbt)
    return by_prob

def find_bests(nbts):
    probs = by_prob_map(nbts)
    bests = {}
    for key in sorted(probs.keys()):
        probs[key].sort(key=lambda x : x['cost'])
        for nbt in probs[key]:
            if nbt['valid']:
                if key not in bests:
                    bests[key] = []
                if len(bests[key]) == 3:
                    break
                bests[key].append(nbt)
    return bests

@app.route('/logs')
def logs():
    exclude_ais = [x for x in request.args.get('exclude_ais', default='').split(',') if x != '']

    nbts = collect_nbts(exclude_ais=exclude_ais)

    for k in range(len(nbts)):
        nbt = nbts[k]
        nbt_path = os.path.relpath(nbt['path'], str(repo_path))
        nbts[k]['vis_url'] = visualizer_url(nbt['prob_id'], nbt_path)
        nbts[k]['name'] = nbt_path

        if nbt['prob_src_path']:
            nbts[k]['prob_src'] = make_url(os.path.relpath(nbt['prob_src_path'], str(repo_path)))
        if nbt['prob_tgt_path']:
            nbts[k]['prob_tgt'] = make_url(os.path.relpath(nbt['prob_tgt_path'], str(repo_path)))
        if nbt['validate_path']:
            nbts[k]['validate_path'] = make_url(os.path.relpath(nbt['validate_path'], str(repo_path)))
        if nbt['javalidate_path']:
            nbts[k]['javalidate_path'] = make_url(os.path.relpath(nbt['javalidate_path'], str(repo_path)))

        t = os.path.getmtime(nbt['path'])
        nbts[k]['date'] = datetime.fromtimestamp(t, JST).strftime('%m/%d %H:%M:%S')
        nbts[k]['t'] = t

    nbts.sort(key=lambda x: x['t'], reverse=True)
    return render_template('logs.html', logs=nbts)

@app.route('/')
def index():
    nbts = collect_nbts()
    probs = collect_probs()
    bests = find_bests(nbts)
    probs_dict = OrderedDict()
    rival_data = get_rival_data()
    not_solveds = []

    for prob in probs:
        prob_id = basename(prob).split('.')[0]
        if prob_id not in bests or bests[prob_id][0]['ai_name'] == 'default':
            not_solveds.append(prob_id)
    not_solveds.sort()

    for k in sorted(bests.keys()):
        nbts = bests[k]
        for nbt in nbts:
            nbt_path = os.path.relpath(nbt['path'], str(repo_path))
            nbt['vis_url'] = visualizer_url(nbt['prob_id'], nbt_path)
            nbt['name'] = nbt_path

            if nbt['prob_src_path']:
                nbt['prob_src'] = make_url(os.path.relpath(nbt['prob_src_path'], str(repo_path)))
            if nbt['prob_tgt_path']:
                nbt['prob_tgt'] = make_url(os.path.relpath(nbt['prob_tgt_path'], str(repo_path)))

            t = os.path.getmtime(nbt['path'])
            nbt['date'] = datetime.fromtimestamp(t, JST).strftime('%m/%d %H:%M:%S')
            nbt['t'] = t
            if k not in probs_dict:
                probs_dict[k] = []
            probs_dict[k].append(nbt)

    return render_template('index.html', probs_dict=probs_dict, rival_data=rival_data, not_solveds=not_solveds)

@app.route('/gitstatus')
def git_status():
    output = ""
    try:
        output += subprocess.check_output(["git", "status"], stderr=subprocess.STDOUT).decode('utf-8').strip()
    except subprocess.CalledProcessError as e:
        output += "Error:" + str(e)
    return render_template('output.html', output=output)

@app.route('/update')
def git_pull():
    output = ""
    try:
        output += subprocess.check_output(
        ["git", "pull", "origin", "master"],
        stderr=subprocess.STDOUT).decode('utf-8').strip()
    except subprocess.CalledProcessError as e:
        output += "Error:" + str(e)
    return render_template('output.html', output=output)

rival_data_cache = []
rival_data_mtime = None
def get_rival_data():
    global rival_data_cache 
    global rival_data_mtime 
    t = os.path.getmtime(str(repo_path / 'misc' / 'full_standings_live.csv'))
    if rival_data_mtime != t:
        rival_data_cache = get_rival_data_internal()
        rival_data_mtime = t
    return rival_data_cache 

def get_rival_data_internal():
    team_id_to_name = {}
    team_data_by_prob = {}
    with open(str(repo_path / 'misc' / 'pubid_to_name.yaml')) as f:
        for line in f:
            team_id, name = line.split(':', 1)
            team_id = team_id.strip("'")
            team_id_to_name[team_id] = name.strip()

    with open(str(repo_path / 'misc' / 'full_standings_live.csv')) as f:
        f.readline() # head
        for line in f:
            team_id, _, prob_id, cost, score = line.split(',', 5)
            if prob_id not in team_data_by_prob:
                team_data_by_prob[prob_id] = []
            team_name = team_id_to_name[team_id]
            team_data_by_prob[prob_id].append({
                'prob_id' : prob_id,
                'team_id' : team_id,
                'team' : team_name,
                'team_short' : team_name[:min(len(team_name),10)],
                'cost' : int(cost),
                'score' : int(score),
            })


    for k in team_data_by_prob.keys():
        team_data_by_prob[k].sort(key=lambda x: x['cost'])
    team_data_by_prob['total'].sort(key=lambda x: x['score'], reverse=True)
    return team_data_by_prob

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, threaded=True, debug=True)

