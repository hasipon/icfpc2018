#!/bin/bash -ex

cd "$(dirname "$0")"
cd ..

echo 'build simulator'
pushd simulator
  go build -o simulator
popd

echo 'build tracer'
pushd tracer
  clang++-3.8 -o tracer -std=c++11 tracer5.cpp
popd

echo 'dump ascii trace files'
pushd out 2>/dev/null
  for ai in *; do

    if ! [[ -d $ai ]]; then
      continue
    fi

    echo $ai

    pushd $ai
      for f in *; do
        if ! [[ $f =~ \.nbt\.gz$ ]]; then
          continue
        fi

        if ! [[ -s $f ]]; then
          continue
        fi

        problemname=${f%.nbt.gz}
        tracefilename="${problemname}.nbt"
        validatefilename="${problemname}.validate"

        echo "$f"

        if ! [[ -e $tracefilename ]]; then
          # unarchive (keep original file)
          gzip -d -k $f
        fi

        modelsrc="../../problemsF/${problemname}_src.mdl"
        modeltgt="../../problemsF/${problemname}_tgt.mdl"

        if ! [[ -e $validatefilename ]]; then
          go run ../../ExecuteTrace/ExecuteTrace.go $modelsrc $modeltgt $tracefilename > $validatefilename
        fi

      done
    popd
  done
popd

#for f in logs/*.ascii; do
#  if [[ -e $f.validate ]]; then
#    continue
#  fi
#
#  if ! [[ -s $f ]]; then
#    continue
#  fi
#
#  if [[ $f =~ logs/([a-zA-Z0-9]+)_([a-zA-Z0-9]+)_tgt.ascii ]]; then
#    echo $f
#    AI_NAME=${BASH_REMATCH[1]}
#    TARGET=${BASH_REMATCH[2]}
#
#    ./simulator/simulator ./problemsL/${TARGET}_tgt.mdl $f &> $f.validate
#  fi
#done
#
#echo 'invoke validate.sh'
#
#./validate.sh

echo 'update submission dir'
./gg.py "update_submission"
if [[ "$(git status submission/nbt 2> /dev/null)" =~ "nothing to commit, working tree clean" ]]; then
  echo "nothing to commit."
else
  git pull origin master
  git add rank
  git add submission/nbt
  git commit -m "update submission by jenkins"
  git push origin master
fi
