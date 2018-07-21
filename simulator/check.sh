#!/bin/bash

cd `dirname $0`

go build -o simulator

if ./simulator ../problemsL/LA001_tgt.mdl LA001.trace | grep -v 335123860; then
    echo 'test failed'
    exit 1
fi
