#!/bin/bash

cd `dirname $0`

if go run *.go 20 LA001.trace LA001.trace | grep -v 335123860; then
    echo 'test failed'
    exit 1
fi
