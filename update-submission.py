#!/usr/bin/env python3
import glob
import pathlib
import shutil
from os.path import splitext, basename, exists
from collections import OrderedDict

repo_path = pathlib.Path(__file__).resolve().parent

def find_best_by_prob():
    logpath = repo_path / 'logs'
    probs_dict = OrderedDict()
    valids = glob.glob(str(logpath) + '/*.ascii.validate')
    logs = {}

    for o in valids:
        base = basename(o).split('.')[0]
        nbt = 'logs/'+ base + '.nbt'
        ai_name, prob_id, _ = base.split('_')
        validv = None
        with open(o) as f:
            s = f.read().strip()
            if s.isdigit():
                validv = int(s)
        if validv:
            obj = {'ai_name' : ai_name, 'nbt_name': nbt, 'cost': validv, 'prob_id': prob_id}
            if prob_id not in logs:
                logs[prob_id] = obj
            elif obj['cost'] < logs[prob_id]['cost']:
                logs[prob_id] = obj
    return logs

def main():
    bests = find_best_by_prob()
    for b in bests.values():
        print(b)
        shutil.copy(b['nbt_name'], str(repo_path / 'submission/nbt/' / (b['prob_id'] + '.nbt')))

if __name__ == '__main__':
    main()
