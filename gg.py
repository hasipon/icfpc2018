#!/usr/bin/env python3
import glob
import gzip
import os
import pathlib
import shutil
import struct
import subprocess
import sys
from os.path import splitext, basename, exists, dirname
from collections import OrderedDict

repo_path = pathlib.Path(__file__).resolve().parent

def collect_nbts(out_path):
    nbts = []

    for path in glob.glob(out_path + '/**/*.nbt.gz', recursive=True):
        prefix = path.split('.')[0]
        prob_id = basename(path).split('.')[0]
        ai_name = basename(dirname(path))
        prob_src_path = str(repo_path / 'problemsF' / prob_id) + '_src.mdl'
        prob_tgt_path = str(repo_path / 'problemsF' / prob_id) + '_tgt.mdl'
        validate_path = prefix + '.validate'
        javalidate_path = prefix + '.javalidate'
        r = 0
        cost = 0
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
                print(x)
                javalid = 0
                #valid = 0
                #valid = 1
                #step = int(s.split(' ')[-1].strip())
                #cost = int(s.split(' ')[-1].strip())

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

def gen_rank_tsv(nbts, output_path):
    os.makedirs(output_path, exist_ok=True)
    probs = by_prob_map(nbts)
    for key in sorted(probs.keys()):
        with open(output_path + '/' + key + '.tsv', 'w') as f:
            f.write('\t'.join(["rank", "ai_name", "cost", "valid", "r", "path"]) + '\n')
            probs[key].sort(key=lambda x : x['cost'])
            rank = 1
            for nbt in probs[key]:
                if not nbt["valid"]:
                    continue
                row = '\t'.join(map(str, [
                    rank,
                    nbt["ai_name"],
                    nbt["cost"],
                    nbt["valid"],
                    nbt["r"],
                    nbt["path"],
                ]))
                f.write(row + '\n')
                rank += 1

def find_bests(nbts):
    probs = by_prob_map(nbts)
    bests = {}
    for key in sorted(probs.keys()):
        probs[key].sort(key=lambda x : x['cost'])
        for nbt in probs[key]:
            print(nbt)
            if nbt['valid']:
                bests[key] = nbt
                break
    return bests

# not working !
def run_tracer(nbts):
    for nbt in nbts:
        if not exists(nbt['ascii_path']):
            cmd = ['./bin/tracer4', str(nbt['r']), nbt['path'], nbt['ascii_path']]
            subprocess.run(cmd)

def update_submission(nbts):
    os.makedirs(str(repo_path / 'submission/nbt/'), exist_ok=True)
    bests = find_bests(nbts)

    with open(str(repo_path / 'submission/list.tsv'), 'w') as f:
        f.write('\t'.join(["prob_id", "ai_name", "cost", "valid", "nbt_path"]) + '\n')

        for nbt in sorted(bests.values(), key=lambda x: x['prob_id']):
            print(nbt)
            cmd = ['gzip', '-d', nbt['path'], '--keep', '--force']
            print(' '.join(cmd))
            subprocess.run(cmd)
            src_path = splitext(nbt['path'])[0]
            dst_path = str(repo_path / 'submission/nbt/' / (nbt['prob_id'] + '.nbt'))
            print('move', src_path, dst_path)
            shutil.move(src_path, dst_path)
            nbt_path = os.path.relpath(nbt['path'], str(repo_path))
            f.write('\t'.join(map(str, [nbt['prob_id'], nbt['ai_name'], nbt['cost'], nbt['valid'], nbt_path])) + '\n')

def find_no_javalid(nbts):
    i = 0
    for nbt in nbts:
        if nbt['javalid'] == None:
            print(nbt['path'])
            i += 1
            if i > 10:
                break

def main():
    op = sys.argv[1]
    out_path = sys.argv[2]

    nbts = collect_nbts(out_path)
    if op == 'update_submission':
        update_submission(nbts)
    elif op == 'gen_rank_tsv':
        gen_rank_tsv(nbts, str(repo_path / 'rank'))
    elif op == 'find_no_javalid':
        find_no_javalid(nbts)
    else:
        print("no such command")

if __name__ == '__main__':
    main()
