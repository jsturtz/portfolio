#!/bin/bash

touch "identity.txt"
A="Some words"
A+=" are extra words"
echo $A

for i in {1..10}; do
  templine=""
  for j in {1..10}; do
    if [[ $i = $j ]]; then
      templine+="1"
    else
      templine+="0"
    fi

    if [[ $j != 10 ]]; then
      templine+="\t"
    fi
  done
  echo -e "Templine: $templine"
  echo -e $templine >> "identity.txt"
done
