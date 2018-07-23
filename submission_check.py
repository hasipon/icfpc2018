#!/usr/bin/env python3
import pathlib

repo_path = pathlib.Path(__file__).resolve().parent

submit = {}
official = {}

for line in open(str(repo_path / 'submission/list.tsv')):
    prob_id, ai_name, cost, valid, javalid ,nbt_path = line.split('\t')
    submit[prob_id] = {
        "prob_id" : prob_id,
        "ai_name" : ai_name,
        "cost" : cost,
        "valid" : valid,
        "javalid" : javalid,
        'nbt_path': nbt_path,
    }
        
for line in open(str(repo_path / 'misc/full_standings_live.csv')):
    team_id, _, prob_id, cost, score = line.split(',')
    if team_id == '0017':
        official[prob_id] = {
            "prob_id" : prob_id,
            "cost" : cost,
            "score" : valid,
        }

for prob_id, o in official.items():
    if prob_id == 'total':
        continue
    s = submit[prob_id]
    print("Problem", s['prob_id']) 
    print(s)
    print(o)
    assert(s['prob_id'] == o['prob_id'])
    assert(s['cost'] == o['cost'])
