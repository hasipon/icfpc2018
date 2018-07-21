#!/bin/bash

cd `dirname $0`
cd ..
git add submission/nbt
git commit -m "update submission"
git push origin master # :(
