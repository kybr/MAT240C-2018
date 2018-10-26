# UNIX-y programs for learning C++ and audio programming

- <http://wiki.c2.com/?UnixWay>
- <https://en.wikipedia.org/wiki/Unix_philosophy>

Generally, these programs generate or operate on streams of floating point numbers in human-readable (ASCII) form.

    phasor.cpp   Generate a phasor
    sine.cpp     Generate a sinuoid
    uniform.cpp  Generate uniformly random numbers
    take.cpp     Only pass on so many inputs
    sort.cpp     Reorganize into ascending or descending order
    info.cpp     Show statistics: count, mean, minimum, maximum
    reverse.cpp  Reverse the order of numbers

Build a program like this:

    make FOO # builds FOO.cpp into FOO.exe

For example, here's how my terminal might look:

    $ cd path/to/unix
    $ make sine
    c++ -std=c++11 -O0  -gsplit-dwarf -Wall -Wextra -Wno-unused-parameter  -I ./ -o sine.exe sine.cpp
    chmod 755 sine.exe
    $ make take
    c++ -std=c++11 -O0  -gsplit-dwarf -Wall -Wextra -Wno-unused-parameter  -I ./ -o take.exe take.cpp
    chmod 755 take.exe
    $ ./sine.exe | ./take 10
    0.022157
    0.044292
    0.066384
    0.088411
    0.110350
    0.132182
    0.153883
    0.175433
    0.196811
    0.217996

