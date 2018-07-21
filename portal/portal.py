import json
import time
import glob
import os
import subprocess
import pathlib
import threading
import shutil
from datetime import datetime, timezone, timedelta
from os.path import splitext, basename, exists
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

@app.route('/logs')
def logs():
    logpath = repo_path / 'logs'
    probs_dict = OrderedDict()
    outs = glob.glob(str(logpath / '*.nbt'))
    outs.sort(key=os.path.getmtime, reverse=True)

    logs = []
    for o in outs:
        base = splitext(basename(o))[0]
        prob_id = base.split('_')[1]
        t = os.path.getmtime(o)
        asci = str(logpath / base) + '.ascii'
        cost = str(logpath / base) + '.cost'
        valid = str(logpath / base) + '.ascii.validate'
        costv = 0
        validv = "None"

        if exists(asci):
            if not exists(cost):
                with open(asci) as f:
                    s = f.readlines()[-1].split(' ')[-1][7:]
                    with open(cost, 'w') as g:
                        g.write(s)
                    costv = s
            else:
                with open(cost) as f:
                    costv = f.read()

            if exists(valid):
                with open(valid) as f:
                    validv = f.read()

        logs.append({
            'name': base + '.nbt',
            'prob': prob_id + '.mdl',
            'prob_id': prob_id,
            'ascii': base + '.ascii',
            'ascii_cost': costv,
            'valid': "ok" if str(validv) == str(costv) else validv,
            'vis_url': visualizer_url(prob_id, basename(o)),
            'date': datetime.fromtimestamp(t, JST).strftime('%m/%d %H:%M:%S'),
        })

    return render_template('logs.html', logs=logs)

def find_best_by_prob():
    logpath = repo_path / 'logs'
    probs_dict = OrderedDict()
    valids = glob.glob(str(logpath) + '/*.ascii.validate')
    logs = {}

    for o in valids:
        base = basename(o).split('.')[0]
        ai_name, prob_id, _ = base.split('_')
        validv = None
        with open(o) as f:
            s = f.read().strip()
            if s.isdigit():
                validv = int(s)
        if validv:
            obj = {'ai_name' : ai_name, 'file_name': o, 'cost': validv, 
                    'vis_url': visualizer_url(prob_id, basename(o)) }
            if prob_id not in logs:
                logs[prob_id] = obj
            elif obj['cost'] < logs[prob_id]['cost']:
                logs[prob_id] = obj
    return logs

@app.route('/')
def index():
    probpath = repo_path / 'problemsL'
    probs_dict = OrderedDict()
    files = glob.glob(str(probpath / '*.mdl'))
    files.sort()
    bests = find_best_by_prob()

    for x in files:
        name = os.path.basename(x)
        prob_id = splitext(name)[0].split('_')[0]
        probs_dict[prob_id] = {'name': name, 'path': x, 'prob_id': prob_id, 'best': None}
        if prob_id in bests:
            probs_dict[prob_id]['best'] = bests[prob_id]

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

