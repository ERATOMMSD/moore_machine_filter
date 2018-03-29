#!/bin/bash

readonly tex_dir=~/Papers/monitorPreprocessing/
readonly fig_dir=${tex_dir}figs/
readonly tbl_dir=${tex_dir}table/

if (($# < 1)); then
    echo "Usage: $0 log_path"
    exit;
fi

cp $1/*.tikz.tex $fig_dir
cp $1/filt-all-*.tsv $tbl_dir
