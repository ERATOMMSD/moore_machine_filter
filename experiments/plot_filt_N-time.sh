#!/bin/bash

# $1: file_name
if (($# < 1)); then
    echo "Usage: $0 input_file [output_file] [plot_type]"
    exit 1
fi


if (($# < 2)); then
    output=/tmp/out.png
else
    output=$2
fi

if (($# < 3)); then
    plottype=png
else
    plottype=$3
fi

# for N in $(cat $1 | cut -d ' ' -f 2 | uniq); do
#     $N
# done

cat $1 | cut -d ' ' -f 1 | sort -n | uniq | xargs -I{} echo -n "\"< awk '\$1=={}' $1\" using 2:3 with lp title \"LENGTH={}\", " | cat <(echo -n 'plot ') - | gnuplot <(echo "set terminal png"; echo 'set output "/tmp/out.png"') -
