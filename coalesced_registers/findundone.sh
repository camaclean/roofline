#!/bin/bash

for i in *.aocx; do
    if [ ! -f pac_a10_${i%.aocx}.log ]; then
	echo $i
    fi
done
