% FILT(1)
% Masaki Waga
% March 2018

# NAME

filt - An experimental implementation of Moore machine filter for timed and untimed pattern matching

# SYNOPSIS

    filt [OPTIONS] -f FILE

# DESCRIPTION

The tool **filt** reads timed or untimed word from standard input, writing masked timed or untimed word to standard output. The tool **filt** is an experimental implementation of Moore machine filter for timed and untimed pattern matching. We believe such a filter is useful in the following situation.

* When we transfer some data via network, we can reduce the data size by filtering out unnecessary part.
* When pattern matching procedure takes too much time, we can reduce the execution time by reducing the input size by filter.

The options are as follows:

**-h**, **--help**
: Print a help message.
**-V**, **--version**
: Print the version.
**-f** *file*, **--automaton** *file*
: Read a (timed) automaton from *file*.
**-u**, **--untimed** (default)
: Untimed filtering mode.
**-t**, **--timed**
: Timed filtering mode.

# Examples

The command:

    filt -f nfa.dot < word.txt
    
read the word from *word.txt* and write the masked word to standard output. It masks the part unnecessary for the pattern matching of the NFA in *nfa.dot*.

The command:

    filt -t -f ta.dot < timed_word.txt
    
read the timed word from *timed_word.txt* and write the masked timed word to standard output. It masks the part unnecessary for the pattern matching of the TA in *ta.dot*.
