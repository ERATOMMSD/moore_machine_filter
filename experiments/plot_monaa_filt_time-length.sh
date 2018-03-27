#!/bin/bash

# $1: file_name


# for N in $(cat $1 | cut -d ' ' -f 2 | uniq); do
#     $N
# done

cat $1 | cut -d ' ' -f 2 | uniq | xargs -I{} echo -n "\"< awk '\$2=={}' $1\" using 1:5 with lp title \"BUFF\\\_SIZE={}\", " | cat <(echo -n 'plot ') - | gnuplot <(echo "set terminal png"; echo 'set output "/tmp/out.png"') -
