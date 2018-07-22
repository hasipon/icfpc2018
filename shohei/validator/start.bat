gzip -c -d ../../out/hasi10/FA186.nbt.gz >temp.nbt
java -jar bin/main.jar ../../problemsF/FA186_tgt.mdl _ temp.nbt temp.result
pause