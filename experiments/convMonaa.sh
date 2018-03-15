#!/bin/bash

if [ $(uname) == Darwin ];then
    g='g'
else
    g=''
fi

cat $@ | ${g}awk -f ./data_descritizer.awk | ${g}tac | ${g}uniq -w 3 | ${g}tac | ${g}awk -f ./pick_edges.awk
