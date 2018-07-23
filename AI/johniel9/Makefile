.PHONY: all
all: FA024.nbt FA037.nbt FA159.nbt

FA024.nbt: johniel9 FA024_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA024_tgt.mdl ./FA024.nbt > output.24

FA037.nbt: johniel9 FA037_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA037_tgt.mdl ./FA037.nbt > output.37

FA159.nbt: johniel9 FA159_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA159_tgt.mdl ./FA159.nbt > output.159

johniel3: johniel9.cpp ../../include/model.hpp
	clang++-3.8 -std=c++11 -O2 johniel3.cpp -o ./johniel9

clean:
	rm johniel9
	rm FA*.nbt
	rm output*
