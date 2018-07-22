#!/bin/bash -ex

pushd simulator
  go build -o simulator
popd

echo validate.sh

./validate.sh
