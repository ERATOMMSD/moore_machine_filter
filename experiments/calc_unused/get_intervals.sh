#!/bin/sh

# monaa -Df ../experiments/dots/torque.dot < /tmp/monaa.input | 
awk '{printf "%s %s ",$1,$5;getline;printf "%s %s ",$1,$5;getline;printf "%s %s\n",$1,$7;getline;}' |  awk '{print $1,$4}' | awk 'NR>1{if($1 != s1){print s1,s2;}}{s1=$1;s2=$2}END{print s1,s2}'
