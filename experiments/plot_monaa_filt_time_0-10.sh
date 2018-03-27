#!/bin/bash

cat $1 | cut -d ' ' -f 2 | sort -n | uniq | head -n 3 | xargs -I{} echo -n "\"< awk '\$2=={}' $1\" using 1:5 with lp title \"BUFF\\\_SIZE={}\", " | cat <(echo -n "plot \"< sort -n -k8,8 $1 | uniq -f 7 | sort -n -k1,1 \" using 1:8 w lp title \"BUFF\\\_SIZE=0\", ") - | gnuplot <(echo "set terminal png"; echo 'set output "/tmp/out.png"'; echo 'set xlabel "length of the input log"'; echo 'set ylabel "execution time [s]"') -
