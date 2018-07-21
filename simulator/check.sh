#!/bin/bash

cd `dirname $0`

if go run *.go ../problemsL/LA001_tgt.mdl LA001.trace | grep -v 335123860; then
    echo 'test failed'
    exit 1
fi

# if go run *.go 2 LA001.trace small-trace | grep -v 332; then
#     echo 'test failed'
#     exit 1
# fi
