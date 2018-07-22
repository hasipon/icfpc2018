#!/bin/bash

for f in logs/*.ascii; do
  if [[ -e $f.validate ]]; then
    continue
  fi

  if ! [[ -s $f ]]; then
    continue
  fi

  if [[ $f =~ logs/([a-zA-Z0-9]+)_([a-zA-Z0-9]+)_tgt.ascii ]]; then
    echo $f
    AI_NAME=${BASH_REMATCH[1]}
    TARGET=${BASH_REMATCH[2]}

    ./simulator/simulator ./problemsL/${TARGET}_tgt.mdl $f &> $f.validate
  fi
done
