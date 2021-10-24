Wiktor Grzankowski
------------------
Program uses C++ STL library to implement a simulator for
NYSA digital systems. For a given input, nysa.cc
writes onto the standard output the truth table of given
signals. For example:
AND 5 4 3 2 1
OR 1 2 3
Means that 5 is the output signal of the AND gate
and 1, 2, 3, 4 are signals that come into the AND gate.
For the OR gate, 1 is the output signal and 2, 3 are 
signals that come into the OR gate.
For this input the program will give the following output:
00000
00010
10100
10110
11000
11010
11100
11111

the first columns is value for 1, second for 2 and so on.
