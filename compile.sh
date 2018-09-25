#!/bin/bash

clang-format-6.0 -i scantest.c
clang-format-6.0 -i advtest.c

gcc scantest.c -lbluetooth -lncurses -o test_scan
gcc advtest.c -lbluetooth -o test_advertise


# ./test_advertise 200 EAC5BDB5DFCB48D2BE60D0A5A71G96E0 5 1 -24
