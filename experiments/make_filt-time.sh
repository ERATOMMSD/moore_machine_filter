#!/bin/bash

cd $(dirname $0)
readonly dir=$(dirname $1)

for file in $dir/$2*.log; do
    basename $file | ggrep -o '[0-9]*' | xargs | awk '{print $3,$1}'
    ./extract_execution_time.sh $file
    ./extract_max_res_set.sh $file
done | xargs -n 1 | xargs -n 4 | awk '{c[$1,$2]+=1;time[$1,$2]+=$3;mem[$1,$2]+=$4}END{{for (key in c){split(key,k,SUBSEP);print k[1],k[2],time[key]/c[key],mem[key]/c[key]}}}' | sort -n -k 1,1 | sort -sn -k 2,2 > $1
