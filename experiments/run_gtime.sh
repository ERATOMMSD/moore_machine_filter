#!/bin/bash

readonly BUFF_SIZES="1 5 10 15 20 25 30 35 40 45 50 55 60 65 70"

readonly date_str=$(date +%m%d%H%M%S)
mkdir -p log/$date_str

for buf_size in $BUFF_SIZES;do
# for buf_size in 1 5;do
    cd ../build && cmake -DCMAKE_BUILD_TYPE=Release -DBUFFER_SIZE=${buf_size} .. && make -j3 filt && cd -

    # Torque
    for input in ~/Data/timedPatternMatching/torque-*.dat; do
    # for input in ~/Data/timedPatternMatching/torque-1000.dat; do
        cat $input | tr 'ba' 'lh' | sed 1d > /tmp/monaa.input

        /usr/local/bin/gtime -v -o log/$date_str/filt-time-torque-${buf_size}-${input##*/}.log ../build/filt -atf ./dots/torque_filter.dot < /tmp/monaa.input > /dev/null
         ../build/filt -atf ./dots/torque_filter.dot < /tmp/monaa.input > log/$date_str/filt-torque-${buf_size}-${input##*/}.txt
        /usr/local/bin/gtime -v -o log/$date_str/filt-monaa-time-torque-${buf_size}-${input##*/}.log bash -c "../build/filt -atf ./dots/torque_filter.dot < /tmp/monaa.input | ../../monaa/build/monaa -Df ./dots/torque.dot" > /dev/null
    done

    # Gear
    for input in ~/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat; do
    #for input in ~/Data/timedPatternMatching/Experiment3_AT-1000-gear-signal.dat; do
        cat $input | sed 1d > /tmp/monaa.input

        /usr/local/bin/gtime -v -o log/$date_str/filt-time-gear-${buf_size}-${input##*/}.log ../build/filt -atf ./dots/gear_filter.dot < /tmp/monaa.input > /dev/null
        ../build/filt -atf ./dots/gear_filter.dot < /tmp/monaa.input > log/$date_str/filt-gear-${buf_size}-${input##*/}.txt
        /usr/local/bin/gtime -v -o log/$date_str/filt-monaa-time-gear-${buf_size}-${input##*/}.log bash -c "../build/filt -atf ./dots/gear_filter.dot < /tmp/monaa.input | ../../monaa/build/monaa -Df ./dots/gear.dot" > /dev/null
    done


    # Accel
    for input in ~/Data/timedPatternMatching/Experiment3_AT-*.tsv; do
    # for input in ~/Data/timedPatternMatching/Experiment3_AT-1000.tsv; do
        ./convMonaa.sh < $input > /tmp/monaa.input

        /usr/local/bin/gtime -v -o log/$date_str/filt-time-accel-${buf_size}-${input##*/}.log ../build/filt -atf ./dots/accel_filter.dot < /tmp/monaa.input > /dev/null
        ../build/filt -atf ./dots/accel_filter.dot < /tmp/monaa.input > log/$date_str/filt-accel-${buf_size}-${input##*/}.txt
        /usr/local/bin/gtime -v -o log/$date_str/filt-monaa-time-accel-${buf_size}-${input##*/}.log bash -c "../build/filt -atf ./dots/accel_filter.dot < /tmp/monaa.input | ../../monaa/build/monaa -Df ./dots/accel.dot" > /dev/null
    done
done


# Torque
for input in ~/Data/timedPatternMatching/torque-*.dat; do
# for input in ~/Data/timedPatternMatching/torque-1000.dat; do
    cat $input | tr 'ba' 'lh' | sed 1d > /tmp/monaa.input

    /usr/local/bin/gtime -v -o log/$date_str/no-filt-monaa-time-torque-${buf_size}-${input##*/}.log bash -c "cat /tmp/monaa.input | ../../monaa/build/monaa -Df ./dots/torque.dot" > /dev/null
done

# Gear
for input in ~/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat; do
#for input in ~/Data/timedPatternMatching/Experiment3_AT-1000-gear-signal.dat; do
    cat $input | sed 1d > /tmp/monaa.input

    /usr/local/bin/gtime -v -o log/$date_str/no-filt-monaa-time-gear-${buf_size}-${input##*/}.log bash -c "cat /tmp/monaa.input | ../../monaa/build/monaa -Df ./dots/gear.dot" > /dev/null
done


# Accel
for input in ~/Data/timedPatternMatching/Experiment3_AT-*.tsv; do
#for input in ~/Data/timedPatternMatching/Experiment3_AT-1000.tsv; do
    ./convMonaa.sh < $input > /tmp/monaa.input

    /usr/local/bin/gtime -v -o log/$date_str/no-filt-monaa-time-accel-${buf_size}-${input##*/}.log bash -c "cat /tmp/monaa.input | ../../monaa/build/monaa -Df ./dots/accel.dot" > /dev/null
done
