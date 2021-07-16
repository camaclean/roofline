#!/bin/bash
var=$(grep -P "Elapsed time: [0-9]{2}:[0-9]{2}:[0-9]{2}" /mnt/scratch/cmaclean/coalesced3/swi_2_coalesced_1_159_double/quartus_sh_compile.log)

total_seconds=0

while read -r line; do
    pattern=".* ([0-9]{2}):([0-9]{2}):([0-9]{2})"
    [[ "$line" =~ $pattern ]]
    echo ${BASH_REMATCH[1]} ${BASH_REMATCH[2]} ${BASH_REMATCH[3]}
    seconds=$((${BASH_REMATCH[1]#0}*60*60 + ${BASH_REMATCH[2]#0}*60 + ${BASH_REMATCH[3]#0}))
    total_seconds=$(($total_seconds + $seconds))
done <<< "$var"

hours=$(($total_seconds/60/60))
minutes=$(( ($total_seconds % (60*60)) / 60 ))
seconds=$(( ($total_seconds % 60) ))
echo $hours:$minutes:$seconds
