#!/bin/bash

pushd /home/ubuntu/icfpc2018

  # update repository
  git pull origin master

  pushd simulator
    go build -o simulator
  popd

  ./validate.sh

popd
