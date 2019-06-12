#!/bin/bash

echo -e "Size\tNoSIMDM\tSIMDM\tNoSIMDR\tSIMDR\n"
size=1000
for x in {1..15}
do
    g++ -o arraymult arraymult.cpp simd.p4.o -lm -fopenmp -DARRAYSIZE=$size
    ./arraymult
    size=$(expr $size \* 2)
done
