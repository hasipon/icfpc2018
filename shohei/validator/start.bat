gzip -c -d ../../out/johniel9/FA022.nbt.gz >temp.nbt
java -jar bin/Main.jar _ ../../problemsF/FA022_tgt.mdl temp.nbt fa.result

gzip -c -d ../../out/hasi16/FD075.nbt.gz >temp.nbt
java -jar bin/Main.jar ../../problemsF/FD075_src.mdl _ temp.nbt fd.result

gzip -c -d ../../out/shioshiota16/FR093.nbt.gz >temp.nbt
java -jar bin/Main.jar ../../problemsF/FR093_src.mdl ../../problemsF/FR093_tgt.mdl temp.nbt fr.result

pause