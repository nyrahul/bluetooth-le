#!/bin/bash

BIN=./app
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../bin/
$BIN
#gdb --args $BIN
