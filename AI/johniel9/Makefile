.PHONY: all
all: FA025.nbt

FA001.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA001_tgt.mdl ./FA001.nbt > output.01

FA017.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA017_tgt.mdl ./FA017.nbt > output.17

FA019.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA019_tgt.mdl ./FA019.nbt > output.19

FA025.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA025_tgt.mdl ./FA025.nbt > output.25

FA050.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA050_tgt.mdl ./FA050.nbt > output.50

FA091.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA091_tgt.mdl ./FA091.nbt > output.91

FA122.nbt: johniel9
	./johniel9  ./INVALID_PATH ./FA122_tgt.mdl ./FA122.nbt > output.122

johniel3: johniel9.cpp ../../include/model.hpp
	clang++-3.8 -std=c++11 -O2 johniel3.cpp -o ./johniel9

clean:
	rm johniel9
	rm FA*.nbt
	rm output*
