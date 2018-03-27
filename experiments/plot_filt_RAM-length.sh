#!/bin/bash

# $1: file_name


# for N in $(cat $1 | cut -d ' ' -f 2 | uniq); do
#     $N
# done

cat $1 | cut -d ' ' -f 2 | sort -n | uniq | xargs -I{} echo -n "\"< awk '\$2=={}' $1\" using 1:4 with lp title \"BUFF\\\_SIZE={}\", " | cat <(echo -n 'plot ') - | gnuplot <(echo "set terminal png"; echo 'set output "/tmp/out.png"'; echo 'set xlabel "length of the input log"'; echo 'set ylabel "Memory consumption"') -
