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

cat $1 | cut -d ' ' -f 2 | sort -n | uniq | head -n 3 | xargs -I{} echo -n "\"< awk '\$2=={}' $1\" using 1:5 with lp title \"BUFF\\\_SIZE={}\", " | cat <(echo -n "plot \"< sort -n -k8,8 $1 | uniq -f 7 | sort -n -k1,1 \" using 1:8 w lp title \"BUFF\\\_SIZE=0\", ") - | gnuplot <(echo "set terminal $plottype"; echo "set output \"$output\""; echo 'set xlabel "length of the input log"'; echo 'set ylabel "execution time [s]"') -
