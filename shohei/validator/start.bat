gzip -c -d ../../out/shioshiota16/FR071.nbt.gz >temp.nbt
java -jar bin/Main.jar _ ../../problemsF/FR071_tgt.mdl temp.nbt temp.result
pause