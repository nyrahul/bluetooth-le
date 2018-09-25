#!/bin/bash

gcc scantest.c -lbluetooth -lncurses -o test_scan
gcc advtest.c -lbluetooth -o test_advertise


# ./test_advertise 200 EAC5BDB5DFCB48D2BE60D0A5A71G96E0 5 1 -24
