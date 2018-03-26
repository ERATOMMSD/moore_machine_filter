#!/bin/bash

readonly RUN_TIMES=5

readonly date_str=$(date +%m%d%H%M%S)
mkdir -p log/$date_str

readonly buf_size=$1

mkdir -p /home/mwaga/Data/timedPatternMatching
cd /home/mwaga/Data/timedPatternMatching && wget https://drive.google.com/uc?export=download&id=1ZHpRv7_fSmzW879unNlEHjx_wpZRYpiD -O torque.tar.xz && tar xvf torque.tar.xz

if (($buf_size != 0)); then
    cd ../build && cmake -DCMAKE_BUILD_TYPE=Release -DBUFFER_SIZE=${buf_size} -DCMAKE_CXX_COMPILER=g++-7 .. && make -j3 filt && cd -

    # Torque
    for input in /home/mwaga/Data/timedPatternMatching/torque-*.dat; do
        # for input in /home/mwaga/Data/timedPatternMatching/torque-1000.dat; do
        cat $input | tr 'ba' 'lh' | sed 1d > /tmp/monaa.input

        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/filt-time-torque-${buf_size}-${input##*/}-$t.log ../build/filt -atf ./dots/torque_filter.dot < /tmp/monaa.input > /dev/null
        done
        ../build/filt -atf ./dots/torque_filter.dot < /tmp/monaa.input > log/$date_str/filt-torque-${buf_size}-${input##*/}.txt
        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/filt-monaa-time-torque-${buf_size}-${input##*/}-$t.log bash -c "../build/filt -atf ./dots/torque_filter.dot < /tmp/monaa.input | /home/mwaga/monaa/build/monaa -Df ./dots/torque.dot" > /dev/null
        done
    done

    # Gear
    for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat; do
        #for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-1000-gear-signal.dat; do
        cat $input | sed 1d > /tmp/monaa.input

        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/filt-time-gear-${buf_size}-${input##*/}-$t.log ../build/filt -atf ./dots/gear_filter.dot < /tmp/monaa.input > /dev/null
        done
        ../build/filt -atf ./dots/gear_filter.dot < /tmp/monaa.input > log/$date_str/filt-gear-${buf_size}-${input##*/}.txt
        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/filt-monaa-time-gear-${buf_size}-${input##*/}-$t.log bash -c "../build/filt -atf ./dots/gear_filter.dot < /tmp/monaa.input | /home/mwaga/monaa/build/monaa -Df ./dots/gear.dot" > /dev/null
        done
    done


    # Accel
    for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-*.tsv; do
        # for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-1000.tsv; do
        ./convMonaa.sh < $input > /tmp/monaa.input

        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/filt-time-accel-${buf_size}-${input##*/}-$t.log ../build/filt -atf ./dots/accel_filter.dot < /tmp/monaa.input > /dev/null
        done
        ../build/filt -atf ./dots/accel_filter.dot < /tmp/monaa.input > log/$date_str/filt-accel-${buf_size}-${input##*/}.txt
        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/filt-monaa-time-accel-${buf_size}-${input##*/}-$t.log bash -c "../build/filt -atf ./dots/accel_filter.dot < /tmp/monaa.input | /home/mwaga/monaa/build/monaa -Df ./dots/accel.dot" > /dev/null
        done
    done
else
    # Torque
    for input in /home/mwaga/Data/timedPatternMatching/torque-*.dat; do
        # for input in /home/mwaga/Data/timedPatternMatching/torque-1000.dat; do
        cat $input | tr 'ba' 'lh' | sed 1d > /tmp/monaa.input

        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/no-filt-monaa-time-torque-${buf_size}-${input##*/}-$t.log bash -c "cat /tmp/monaa.input | /home/mwaga/monaa/build/monaa -Df ./dots/torque.dot" > /dev/null
        done
    done

    # Gear
    for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat; do
        #for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-1000-gear-signal.dat; do
        cat $input | sed 1d > /tmp/monaa.input

        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/no-filt-monaa-time-gear-${buf_size}-${input##*/}-$t.log bash -c "cat /tmp/monaa.input | /home/mwaga/monaa/build/monaa -Df ./dots/gear.dot" > /dev/null
        done
    done


    # Accel
    for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-*.tsv; do
        #for input in /home/mwaga/Data/timedPatternMatching/Experiment3_AT-1000.tsv; do
        ./convMonaa.sh < $input > /tmp/monaa.input

        for t in $(seq $RUN_TIMES); do
            /usr/bin/time -v -o log/$date_str/no-filt-monaa-time-accel-${buf_size}-${input##*/}-$t.log bash -c "cat /tmp/monaa.input | /home/mwaga/monaa/build/monaa -Df ./dots/accel.dot" > /dev/null
        done
    done
fi
