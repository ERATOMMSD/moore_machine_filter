Evaluation
==========

The Data Set We Use
-------------------

All inputs are from [Waga et al., FORMATS'17].

* Torque
    * Timed words: ~/Data/timedPatternMatching/torque-*.dat `| tr 'ba' 'lh' | sed 1d`
        * Probably we should reduce the number of inputs because it is too much.
    * Pattern: ./dots/torque_filter.dot
        * We run `monaa -D`
* ~~Settling~~
    * I do not think this is a good example because the pattern for filter is a DFA.
    * Timed words: ~/Data/timedPatternMatching/BenchmarkPTC-Model1-*-signal.dat
    * Pattern: not yet
* Gear
    * Timed words: ~/Data/timedPatternMatching/Experiment3_AT-*-gear-signal.dat `| sed 1d`
    * Pattern: ./dots/gear_filter.dot
        * We run `monaa -D`
* Accel
    * Timed words: ~/Data/timedPatternMatching/Experiment3_AT-*.tsv
    * Pattern: ./dots/accel_filter.dot
        * We run `monaa`

What we measure
---------------

* result of `/usr/bin/time -v -o [log]  ../build/filt -taf < [timed_word] > /dev/null 2>&1`
    * For each:
        * timed word
        * pattern
        * buffer size
    * We can get:
        * execution time
        * memory usage
* result of `../build/filt -taf < [timed_word] > [log]`
    * For each:
        * timed word
        * pattern
        * buffer size
    * We can get:
        * reduction rate (by `wc -l`)
* result of `/usr/bin/time -v -o [log]  (cat [timed_word] | ../build/filt -taf [timed_automaton] | ../../monaa/build/monaa -f [timed_automaton]) > /dev/null 2>&1`
* result of `/usr/bin/time -v -o [log]  (cat [timed_word] | ../../monaa/build/monaa -f [timed_automaton]) > /dev/null 2>&1`
    * For each:
        * timed word
        * pattern
        * buffer size
    * We can get:
        * over all performance comparison 

