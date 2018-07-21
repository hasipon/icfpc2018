import json
import time
import glob
import os
import datetime
import subprocess
import pathlib
import threading
import shutil
from collections import OrderedDict
from flask import Flask, request, render_template, abort, send_from_directory, make_response
from werkzeug.utils import secure_filename

static_path = pathlib.Path(__file__).resolve().parent / 'static'
repo_path = pathlib.Path(__file__).resolve().parent.parent
app = Flask(__name__, static_folder = str(static_path), static_url_path='')
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0

@app.after_request
def add_header(response):
    if 'Expires' in response.headers:
        del response.headers['Expires']
    response.headers['Cache-Control'] = 'no-store'
    return response

@app.route('/problemsL/<name>', methods=['GET'])
def get_problems_l(name):
    return send_from_directory(repo_path / 'problemsL', name)

@app.route('/')
def index():
    probpath = repo_path / 'problemsL'
    probs_dict = OrderedDict()
    files = glob.glob(str(probpath / '*.mdl'))
    files.sort()
    for x in files:
        name = os.path.basename(x)
        probs_dict[name] = {'name': name, 'path': x}

    try:
        for line in open (str(repo_path / 'problemsL' / 'problemsL.txt')):
            name, other = line.split('::')
            probs_dict[name]['desc'] = other
    except (ValueError, FileNotFoundError) as e:
        print(e)
        pass

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
        now = datetime.datetime.now()
        filename = secure_filename("{0:%Y%m%d_%H%M%S}-{1}".format(now, f.filename))
        path = str(static_path / 'logs' / f.filename)
        f.save(path)

        cost, out = calc_cost(prob, path)
        update_best_solution(prob, path, cost)
        return make_response(out, 200)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, threaded=True, debug=True)
