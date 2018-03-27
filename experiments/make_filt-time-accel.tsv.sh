#!/bin/bash

cd $(dirname $0)
readonly dir=$(dirname $1)

for file in $dir/$2*.log; do
    basename ${file%%.*} | ggrep -o '[0-9]*' | gsed 'N;N;s/\n/ /g' | awk '{print $3,$1}'
    ./extract_execution_time.sh $file
    ./extract_max_res_set.sh $file
done | xargs -n 1 | xargs -n 4 > $1
