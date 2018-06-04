#!/bin/bash
## A script to calculate the average delay caused by our filter

if (($# < 1)); then
    echo "usage: $0 <log_dir>"
    exit
fi

readonly log_dir=$1
readonly log_types="torque gear accel"

for type in $log_types; do
    cat $log_dir/filt-all-$type.tsv | awk -v type=$type '$2==10{s1+=$1;s3+=$3}END{print type":",s3/s1*10 * 1000 * 1000,"us"}'
done

