CC=clang++-3.8
FLAG=-std=c++11

all: johniel2.cpp ../../include/model.hpp
	$(CC) $(FLAG) johniel2.cpp -o ./johniel2
	./johniel2 ../../problemsL/LA019_tgt.mdl ./LA019.nbt	

large: all
	./johniel2 ../../problemsL/LA163_tgt.mdl ./LA163.nbt	
