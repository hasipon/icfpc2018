.PHONY: all
all: FA001.nbt FA019.nbt FA050.nbt

FA001.nbt: johniel7
	./johniel7  ./INVALID_PATH ../FA001_tgt.mdl ./FA001.nbt > output.01

FA019.nbt: johniel7
	./johniel7  ./INVALID_PATH ../FA019_tgt.mdl ./FA019.nbt > output.19

FA050.nbt: johniel7
	./johniel7  ./INVALID_PATH ../FA050_tgt.mdl ./FA050.nbt > output.50

johniel3: johniel7.cpp ../../include/model.hpp
	clang++-3.8 -std=c++11 -O2 johniel3.cpp -o ./johniel7

clean:
	rm johniel7
	rm FA001.nbt
	rm FA019.nbt
	rm FA050.nbt
