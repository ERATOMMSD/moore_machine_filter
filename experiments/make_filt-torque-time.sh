#!/bin/bash

cd $(dirname $0)
readonly dir=$(dirname $1)

for file in $dir/$2*.log; do
    basename ${file%%.*} | ggrep -o '[0-9]*' | xargs -n 2 | awk '{print $2,$1}'
    ./extract_execution_time.sh $file
    ./extract_max_res_set.sh $file
done | xargs -n 4 | sort -n -k 1,1 | sort -sn -k 2,2 > $1
