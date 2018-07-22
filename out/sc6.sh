#!/bin/bash
set -e
cd $(dirname $0)
ls */*.gz | perl sc6.pl > Makefile
make -k
