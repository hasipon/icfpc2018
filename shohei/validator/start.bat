gzip -c -d ../../out/hasi10/FA186.nbt.gz >temp.nbt
java -jar bin/Main.jar _ ../../problemsF/FA186_tgt.mdl temp.nbt temp.result
pause