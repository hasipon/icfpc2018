.PHONY: all
all: FA001.nbt FA019.nbt FA137.nbt

FA001.nbt: johniel9 FA001_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA001_tgt.mdl ./FA001.nbt > output.01

FA017.nbt: johniel9 FA017_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA017_tgt.mdl ./FA017.nbt > output.17

FA019.nbt: johniel9 FA019_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA019_tgt.mdl ./FA019.nbt > output.19

FA025.nbt: johniel9 FA025_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA025_tgt.mdl ./FA025.nbt > output.25

FA050.nbt: johniel9 FA050_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA050_tgt.mdl ./FA050.nbt > output.50

FA091.nbt: johniel9 FA091_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA091_tgt.mdl ./FA091.nbt > output.91

FA104.nbt: johniel9 FA104_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA104_tgt.mdl ./FA104.nbt > output.104

FA122.nbt: johniel9 FA122_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA122_tgt.mdl ./FA122.nbt > output.122

FA137.nbt: johniel9 FA137_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA137_tgt.mdl ./FA137.nbt > output.137

FA181.nbt: johniel9 FA181_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA181_tgt.mdl ./FA181.nbt > output.181

johniel3: johniel9.cpp ../../include/model.hpp
	clang++-3.8 -std=c++11 -O2 johniel3.cpp -o ./johniel9

clean:
	rm johniel9
	rm FA*.nbt
	rm output*
