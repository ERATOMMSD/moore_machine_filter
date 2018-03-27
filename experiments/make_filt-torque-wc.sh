#!/bin/bash

cd $(dirname $0)
readonly dir=$(dirname $1)

for file in $dir/$2*.txt; do
    basename $file | ggrep -o '[0-9]*' | xargs  
    cat $file | wc -l 
done | xargs -n 1 | xargs -n 3 | awk '{print $2,$1,$3}' | sort -n -k 1,1 | sort -sn -k 2,2 > $1
