.PHONY: all
all: FA024.nbt FA037.nbt FA159.nbt FA181.nbt FA186.nbt  FA065.nbt

FA024.nbt: johniel9 FA024_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA024_tgt.mdl ./FA024.nbt > output.24

FA037.nbt: johniel9 FA037_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA037_tgt.mdl ./FA037.nbt > output.37

FA065.nbt: johniel9 FA065_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA065_tgt.mdl ./FA065.nbt > output.065

FA159.nbt: johniel9 FA159_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA159_tgt.mdl ./FA159.nbt > output.159

FA181.nbt: johniel9 FA181_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA181_tgt.mdl ./FA181.nbt > output.181

FA186.nbt: johniel9 FA186_tgt.mdl
	./johniel9  ./INVALID_PATH ./FA186_tgt.mdl ./FA186.nbt > output.186

johniel3: johniel9.cpp ../../include/model.hpp
	clang++-3.8 -std=c++11 -O2 johniel3.cpp -o ./johniel9

clean:
	rm johniel9
	rm FA*.nbt
	rm output*
