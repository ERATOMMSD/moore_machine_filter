#!/bin/bash

# $1: file_name
if (($# < 1)); then
    echo "Usage: $0 [file_name]"
    exit 1
fi


# for N in $(cat $1 | cut -d ' ' -f 2 | uniq); do
#     $N
# done

cat $1 | cut -d ' ' -f 1 | sort -n | uniq | xargs -I{} echo -n "\"< awk '\$1=={}' $1\" using 2:7 with lp title \"LENGTH={}\", " | cat <(echo -n 'plot ') - | gnuplot <(echo "set terminal png"; echo 'set output "/tmp/out.png"'; echo 'set xlabel "size of buffer"'; echo 'set ylabel "length of the filtered log"') -
