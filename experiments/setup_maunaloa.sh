#!/bin/bash

## Set up input data
mkdir -p /home/mwaga/Data/timedPatternMatching
cd /home/mwaga/Data/timedPatternMatching && wget 'https://drive.google.com/uc?export=download&id=1ZHpRv7_fSmzW879unNlEHjx_wpZRYpiD' -O torque.tar.xz && tar xvf torque.tar.xz

## Set up latest MONAA
cd /home/mwaga && rm -rf monaa && git clone https://github.com/MasWag/monaa.git && cd monaa && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-7 .. && make monaa -j2
