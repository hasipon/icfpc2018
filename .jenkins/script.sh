#!/bin/bash

# check simulator
./simulator/check.sh

# update repository
pushd /home/ubuntu/icfpc2018
git pull origin master
popd
