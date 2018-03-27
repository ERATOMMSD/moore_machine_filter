#!/bin/bash

cd $(dirname $0)
readonly dir=$(dirname $1)

for file in $dir/$2*.log; do
    basename ${file%%.*} | grep -o '[0-9]\+' | tail -n 1
    ./extract_execution_time.sh $file
done | xargs -n 2 | sort -n > /tmp/no-filt-time.tmp
for a in {1..15}; do
    cat /tmp/no-filt-time.tmp;
done  > $1
