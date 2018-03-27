#!/bin/bash

# $1: file_name
if (($# < 1)); then
    echo "Usage: $0 input_file [output_file] [plot_type]"
    exit 1
fi


# for N in $(cat $1 | cut -d ' ' -f 2 | uniq); do
#     $N
# done

cat $1 | cut -d ' ' -f 2 | sort -n | uniq | xargs -I{} echo -n "\"< awk '\$2=={}' $1\" using 1:7 with lp title \"BUFF\\\_SIZE={}\", " | cat <(echo -n 'plot ') - | gnuplot <(echo "set terminal png"; echo 'set output "/tmp/out.png"') -
