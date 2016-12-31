#!/bin/bash
for file in $(diff <(ls ./Examples/*_*) <(ls ./Examples/*.sc) | grep '<' | cut -f 2 -d ' ');  do
echo $file.sc
./scc $file.sc 2> /dev/null
#cat a.out
#cat $file
diff a.out $file
done
