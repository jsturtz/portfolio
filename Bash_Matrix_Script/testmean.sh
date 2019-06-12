#!/bin/bash

# 
i=1
tempfile="temp$$"
read -r firstline<m1.txt
for num in $firstline ; do
  cat m1.txt | cut -f $i > $tempfile
  
  sum=0
  count=0
  while read line ; do
    sum=$(expr $line + $sum)
    ((count++))
  done < $tempfile
    
  mean=$(expr $sum / $count)
  echo -e -n "$mean\t"
  ((i++))
done
echo
rm $tempfile
