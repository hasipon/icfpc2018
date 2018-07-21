#!/bin/bash -x
prob=$1
file=$2

http_code=$(curl -v --compressed -w '%{http_code}' -F "solution=@${file}" "http://18.179.226.203:5000/upload/${prob}")

if [[ $http_code -eq 200 ]]; then
  exit 0
else
  exit 1
fi

