
gzip -c -d ../../out/johniel8/FA036.nbt.gz >temp.nbt
java -jar bin/Main.jar _ ../../problemsF/FA036_tgt.mdl temp.nbt temp.result
pause