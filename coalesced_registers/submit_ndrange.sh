#!/bin/bash

set -eu

skip_aoco=

declare -A typemap=( [float]=f [double]=d [int]=i [long]=l )

readarray -t completed < submitted.txt

firststage_only=

if [ "${1:-}" == "gettimes" ]; then
    gettimes=yes
else
    gettimes=
fi

gettime() {
    local t total_seconds pattern seconds hours minutes
    t=$(grep -P "Elapsed time: [0-9]{2}:[0-9]{2}:[0-9]{2}" ${1})

    total_seconds=0

    while read -r line; do
        pattern=".* ([0-9]{2}):([0-9]{2}):([0-9]{2})"
        [[ "$line" =~ $pattern ]]
        seconds=$((${BASH_REMATCH[1]#0}*60*60 + ${BASH_REMATCH[2]#0}*60 + ${BASH_REMATCH[3]#0}))
        total_seconds=$(($total_seconds + $seconds))
    done <<< "$var"

    hours=$(($total_seconds/60/60))
    minutes=$(( ($total_seconds % (60*60)) / 60 ))
    seconds=$(( ($total_seconds % 60) ))
    echo $hours:$minutes:$seconds\t$total_seconds
}

if [ ! -z "$gettimes" ]; then
    echo "" > "times.txt"
fi

submit() {
    local i buffer type simd comp name els_per_bank
    buffer=$1
    type=$2
    simd=$3
    comp=$4
    els_per_bank=${5:-}
    [ -z "${els_per_bank}" ] && name="ndrange_${buffer}_coalesced_${simd}_${comp}_${type}" || name="ndrange_${buffer}_coalesced_${simd}_${els_per_bank}_${comp}_${type}"
    [ -z "${els_per_bank}" ] && shortname="n_${buffer}_c_${simd}_${comp}_${typemap[$type]}" || shortname="n_${buffer}_c_${simd}_${els_per_bank}_${comp}_${typemap[$type]}"
    export buffer type simd comp firststage_only els_per_bank name

    [ -f "$name.aocx" ] && return
    [ ! -z "$skip_aoco" ] && [ -f "$name.aoco" ] && return
    for i in "${completed[@]}"; do
        [[ "$i" == $shortname ]] && return
    done
    qsub -V -N "$shortname" ndrange.pbs
    echo "$shortname" >> submitted.txt

    if [ -f /mnt/scratch/cmaclean/coalesced3/$name/quartus_sh_compile.log ]; then
        file=/mnt/scratch/cmaclean/coalesced3/$name/quartus_sh_compile.log
        echo yes3
    elif [ -f /mnt/scratch/cmaclean/coalesced2/$name/quartus_sh_compile.log ]; then
        file=/mnt/scratch/cmaclean/coalesced2/$name/quartus_sh_compile.log
        echo yes2
    elif [ -f /mnt/scratch/cmaclean/coalesced/$name/quartus_sh_compile.log ]; then
        file=/mnt/scratch/cmaclean/coalesced/$name/quartus_sh_compile.log
        echo yes
    fi

    if [ ! -z "$gettimes" ]; then
        echo "ndrange\t${buffer}\tcoalesced\t$unroll\t${els_per_bank:-0}\t$comp\t$type\t$(gettime $file)" >> "times.txt"
    fi
}

submit 1 float 1   1    # y
submit 1 float 1   3033 # y
#submit 1 float 1 3034  # too big
submit 2 float 1   1    # y
submit 1 float 2   1    # y
submit 2 float 2   757  # y
#submit 2 float 2   758 # too big
submit 1 float 2   1515 # y
#submit 1 float 2   1516 # too big
submit 1 float 4   1    # y
submit 2 float 4   1    # y
submit 2 float 4   377  # y
#submit 2 float 4   378  # too big
submit 1 float 4   757  # y
#submit 1 float 4   758  # too big
submit 1 float 8   1    # y
submit 2 float 8   1    # y
submit 2 float 8   187  # y
#submit 2 float 8   188 # too big
submit 1 float 8   377  # y
#submit 1 float 8   378 # too big
submit 1 float 16  1    # y
submit 2 float 16  1    # y
submit 2 float 16  92   # y
submit 2 float 16  93   # XXX
submit 1 float 16  187  # y
#submit 1 float 16  188  # too big


# submit local float 1 1 1
# submit local float 1 3032 1
# submit local float 2 1 1
# submit local float 2 1 2
# submit local float 2 1516 1
# submit local float 2 1516 2
# submit local float 4 1 1
# submit local float 4 1 4
# submit local float 4 758 1
# submit local float 4 758 4
# submit local float 8 1 1
# submit local float 8 1 8
# submit local float 8 379 1
# submit local float 8 379 8
# submit local float 16 1 1
# submit local float 16 1 16
# submit local float 16 189 1
# submit local float 16 189 16
# submit local float 32 1 1
# submit local float 32 1 32
# submit local float 32 94 1
# submit local float 32 94 32
# submit local float 64 1 1
# submit local float 64 1 64
# submit local float 64 47 1
# submit local float 64 47 64
# submit local float 128 1 1
# submit local float 128 1 128
# submit local float 128 23 1
# submit local float 128 23 128
# submit local float 256 1 1
# submit local float 256 1 256
# submit local float 256 11 1
# submit local float 256 11 256
# submit local float 512 1 1
# submit local float 512 1 512
# submit local float 512 5 1
# submit local float 512 5 512
