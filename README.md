
Moore Machine Filter
====================

[![Boost.Test](https://github.com/ERATOMMSD/moore_machine_filter/actions/workflows/unit_test.yml/badge.svg)](https://github.com/ERATOMMSD/moore_machine_filter/actions/workflows/unit_test.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](./LICENSE)

An experimental implementation of Moore machine filter for timed and untimed pattern matching.
This implementation is tested on Ubuntu (18.04 and 20.04) and macOS 10.15 Catalina.

[![asciicast](https://asciinema.org/a/483213.svg)](https://asciinema.org/a/483213)

Getting Started
------------

### Requirements

* C++ compiler supporting C++17 and the corresponding libraries.
* Boost (>= 1.59)
* Eigen
* CMake

### Building

```sh
mkdir build
cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUFFER_SIZE=10 .. && make
```

### Configuration

BUFFER_SIZE
: The size of buffer in the Moore machine (default: 10).

Usage
-----

### Synopsis

    filt [OPTIONS] -f FILE

### Options

**-h**, **--help** Print a help message. <br />
**-V**, **--version** Print the version.  <br />
**-f** *file*, **--automaton** *file* Read a (timed) automaton from *file*.  <br />
**-u**, **--untimed** (default) Untimed filtering mode. <br />
**-t**, **--timed** Timed filtering mode. <br />

Syntax of Timed Automata
------------------------

You can use [DOT language](http://www.graphviz.org/content/dot-language) to represent a timed automaton or a NFA. For the timing constraints and other information, you can use the following custom attributes.

<table>
<thead>
<tr class="header">
<th></th>
<th>attribute</th>
<th>value</th>
<th>description</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>vertex</td>
<td>init</td><td>0 or 1</td><td><tt>init=1</tt> if the state is initial</td></tr>
<tr class="even">
<td>vertex</td><td>match</td><td>0 or 1</td><td><tt>match=1</tt> if the state is accepting</td>
</tr>
<tr class="odd">
<td>edge</td><td>label</td><td>[a-z], [A-Z]</td><td>the value represents the event on the transition</td>
</tr>
<tr class="even">
<td>edge</td><td>reset</td><td>a list of integers</td><td>the set of variables reset after the transition</td>
</tr>
<tr class="odd">
<td>edge</td><td>guard</td><td>a list of inequality constraints</td><td>the guard of the transition</td>
</tr>
</tbody>
</table>

References
-------------

* Waga, Masaki, and Ichiro Hasuo. "Moore-machine filtering for timed and untimed pattern matching." IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems 37.11 (2018): 2649-2660.
