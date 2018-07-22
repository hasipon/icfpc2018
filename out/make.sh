#!/bin/bash
set -e
cd $(dirname $0)
ls ../nbts/*.nbt | perl make.pl > Makefile
make
