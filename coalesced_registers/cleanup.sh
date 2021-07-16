#!/bin/bash

set -eu

backup="/mnt/scratch/cmaclean/coalesced_registers"

mkdir -p ${backup%/}

for i in *.aocx; do
    dir=${i%.aocx}
    if [ -d "$dir" ]; then
	[ -d "${backup%/}/${dir}" ] && echo "Already exists in backup: $dir" && continue || echo $dir
	rm -r $dir || ( echo "Delete failed"; exit 2 )
    fi
done
