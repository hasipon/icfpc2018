all: FA001.nbt FA019.nbt

FA001.nbt: johniel5
	./johniel5  ./INVALID_PATH ../../problemsF/FA001_tgt.mdl ./FA001.nbt > output

FA019.nbt: johniel5
	./johniel5  ./INVALID_PATH ../../problemsF/FA019_tgt.mdl ./FA019.nbt > output

johniel3: johniel5.cpp ../../include/model.hpp
	clang++-3.8 -std=c++11 -O2 johniel3.cpp -o ./johniel5

clean:
	rm johniel5
	rm FA001.nbt
	rm FA019.nbt
