#!/bin/bash

gcc -Wall -W -O2 -fopenmp -o "sad14_04_template" "sad14_04_template.c"

./ue04 10000 20 0.5 1
./ue04 10000 20 0.5 2
./ue04 10000 20 0.5 3
./ue04 10000 20 0.5 4
./ue04 10000 20 0.5 5
./ue04 10000 20 0.5 6
./ue04 10000 20 0.5 7
./ue04 10000 20 0.5 8
./ue04 10000 20 0.5 9
./ue04 10000 20 0.5 10