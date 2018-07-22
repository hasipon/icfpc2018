#!/bin/bash -ex

cd "$(dirname "$0")"
cd ..

# echo 'build simulator'
# pushd simulator
#   go build -o simulator
# popd

echo 'build tracer'
pushd tracer
  clang++ -o tracer -std=c++11 tracer4.cpp
popd

echo 'dump ascii trace files'
pushd out 2>/dev/null
  for ai in *; do
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
        traceasciifilename="${problemname}.ascii"
        validatefilename="${problemname}.validate"

        if [[ -e $traceasciifilename ]]; then
          continue
        fi

        echo $f

        # unarchive (keep original file)
        gzip -d -k $f

        modelsrc="../../problemsF/${problemname}_src.mdl"
        modeltgt="../../problemsF/${problemname}_tgt.mdl"

        resolution=$(cat ../../problemsF/${problemname}.r)

        # convert to ascii file
        ../../tracer/tracer $resolution $tracefilename $traceasciifilename

        # # validate
        # ../../simulator/simulator $modelsrc $modeltgt $traceasciifilename &> $validatefilename
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
