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

def visualizer_url(prob_id, solution_path):
    return 'http://18.179.226.203/shohei/visualizer/bin/index.html#{"model":"' + prob_id + '","file":"' + solution_path + '"}'

@app.after_request
def add_header(response):
    if 'Expires' in response.headers:
        del response.headers['Expires']
    response.headers['Cache-Control'] = 'no-store'
    return response

@app.route('/problemsL/<name>', methods=['GET'])
def get_problems_l(name):
    return send_from_directory(repo_path / 'problemsL', name)

def collect_nbts():
    nbts = []

    for path in glob.glob(str(repo_path / 'out') + '/**/*.nbt.gz', recursive=True):
        prefix = path.split('.')[0]
        prob_id = basename(path).split('.')[0]
        ai_name = basename(dirname(path))
        prob_src_path = str(repo_path / 'problemsF' / prob_id) + '_src.mdl'
        prob_tgt_path = str(repo_path / 'problemsF' / prob_id) + '_tgt.mdl'
        ascii_path = prefix + '.ascii'
        validate_path = prefix + '.validate'
        r = 0
        cost = 0
        valid = 0
        updated = 0

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
                s = f.read().strip()
                if s.isdigit():
                    cost = int(s)
                    valid = 1

        nbts.append({
            "path" : path,
            "prefix" : prefix,
            "prob_id" : prob_id,
            "ai_name" : ai_name,
            "prob_src_path" : prob_src_path,
            "prob_tgt_path" : prob_tgt_path,
            "ascii_path" : ascii_path,
            "validate_path" : ascii_path,
            "r" : r,
            "cost" : cost,
            "valid" : valid,
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
                bests[key] = nbt
                break
    return bests

@app.route('/logs')
def logs():
    nbts = collect_nbts()

    for k in range(len(nbts)):
        nbt = nbts[k]
        nbt_path = os.path.relpath(nbt['path'], str(repo_path))
        nbts[k]['vis_url'] = visualizer_url(nbt['prob_id'], nbt_path)
        nbts[k]['name'] = nbt_path
        nbts[k]['ascii'] = os.path.relpath(nbt['ascii_path'], str(repo_path))

        if nbt['prob_src_path']:
            nbts[k]['prob_src'] = os.path.relpath(nbt['prob_src_path'], str(repo_path))
        if nbt['prob_tgt_path']:
            nbts[k]['prob_tgt'] = os.path.relpath(nbt['prob_tgt_path'], str(repo_path))

        t = os.path.getmtime(nbt['path'])
        nbts[k]['date'] = datetime.fromtimestamp(t, JST).strftime('%m/%d %H:%M:%S')
        nbts[k]['t'] = t

    nbts.sort(key=lambda x: x['t'], reverse=True)
    return render_template('logs.html', logs=nbts)

@app.route('/')
def index():
    nbts = collect_nbts()
    bests = find_bests(nbts)
    probs_dict = OrderedDict()

    for k in sorted(bests.keys()):
        nbt = nbts[k]
        nbt_path = os.path.relpath(nbt['path'], str(repo_path))
        nbts[k]['vis_url'] = visualizer_url(nbt['prob_id'], nbt_path)
        nbts[k]['name'] = nbt_path
        nbts[k]['ascii'] = os.path.relpath(nbt['ascii_path'], str(repo_path))

        if nbt['prob_src_path']:
            nbts[k]['prob_src'] = os.path.relpath(nbt['prob_src_path'], str(repo_path))
        if nbt['prob_tgt_path']:
            nbts[k]['prob_tgt'] = os.path.relpath(nbt['prob_tgt_path'], str(repo_path))

        t = os.path.getmtime(nbt['path'])
        nbts[k]['date'] = datetime.fromtimestamp(t, JST).strftime('%m/%d %H:%M:%S')
        nbts[k]['t'] = t
        probs_dict[k] = nbts[k]

    return render_template('index.html', probs_dict=probs_dict)

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

def calc_cost(prob, sol):
    cost = 123
    out = "--"
    return cost, out 

def update_best_solution(prob, path, cost):
    sol_path = str(repo_path / 'submission' / 'nbt' / prob) + '.nbt'
    updated = False
    if os.path.exists(sol_path):
        best, _ = calc_cost(prob, sol_path)
        if cost < best:
            updated = True
            shutil.copy(path, sol_path)
    else:
        updated = True
        print(path, sol_path)
        shutil.copy(path, sol_path)
    return updated

@app.route('/upload/<prob>', methods=['POST'])
def upload(prob):
    if 'solution' not in request.files:
        print("solution not found")
        return abort(403)
    f = request.files['solution']
    if f.filename == '':
        return abort(403)
    if f:
        now = datetime.now()
        filename = secure_filename("{0:%Y%m%d_%H%M%S}-{1}".format(now, f.filename))
        path = str(static_path / 'logs' / f.filename)
        f.save(path)

        cost, out = calc_cost(prob, path)
        update_best_solution(prob, path, cost)
        return make_response(out, 200)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, threaded=True, debug=True)

