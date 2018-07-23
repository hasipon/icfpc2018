gzip -c -d ../../out/johniel8/FA024.nbt.gz >temp.nbt
java -jar bin/Main-debug.jar _ ../../problemsF/FA024_tgt.mdl temp.nbt temp.result
pause