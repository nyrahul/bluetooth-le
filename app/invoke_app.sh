#!/bin/bash

BIN=./app
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../bin/linux
$BIN
#gdb --args $BIN
