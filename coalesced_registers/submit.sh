#!/bin/bash

set -eu

skip_aoco=

declare -A typemap=( [float]=f [double]=d [int]=i [long]=l [signed char]=sc [char]=c )

touch submitted.txt

readarray -t completed < submitted.txt

firststage_only=

if [ "${1:-}" == "gettimes" ]; then
    gettimes=yes
    rm "times.txt"
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
    done <<< "$t"

    hours=$(($total_seconds/60/60))
    minutes=$(( ($total_seconds % (60*60)) / 60 ))
    seconds=$(( ($total_seconds % 60) ))
    #echo -e "${hours}:${minutes}:${seconds}\t${total_seconds}"
    printf "%02d:%02d:%02d\t%d" ${hours} ${minutes} ${seconds} ${total_seconds}
}


submit() {
    local i buffer type unroll comp name els_per_bank directory file
    buffer=$1
    type=$2
    unroll=$3
    comp=$4
    els_per_bank=${5:-}
    export buffer type unroll comp firststage_only els_per_bank
    [ -z "${els_per_bank}" ] && name="swi_${buffer}_coalesced_${unroll}_${comp}_${type// /_}" || name="swi_${buffer}_coalesced_${unroll}_${els_per_bank}_${comp}_${type// /_}"
    [ -z "${els_per_bank}" ] && shortname="s_${buffer}_c_${unroll}_${comp}_${typemap[$type]}" || shortname="s_${buffer}_c_${unroll}_${els_per_bank}_${comp}_${typemap[$type]}"

    if [ -z "$gettimes" ]; then
        [ -f "$name.aocx" ] && return
        [ ! -z "$skip_aoco" ] && [ -f "$name.aoco" ] && return
        for i in "${completed[@]:-}"; do
            [[ "$i" == $shortname ]] && return
        done
        echo $name
        qsub -V -N "$shortname" swi.pbs
        echo "$shortname" >> submitted.txt
    else
        if [ -d /mnt/scratch/cmaclean/coalesced_register/save/$name ]; then
            directory=/mnt/scratch/cmaclean/coalesced_register/save/$name
        else
            echo "${name} not found"
            return
        fi

        file=${directory}/quartus_sh_compile.log

        mkdir -p /mnt/scratch/cmaclean/roofline_coalesced_register

        ln -snf $directory /mnt/scratch/cmaclean/roofline_coalesced_register/$name || true

        echo -e "swi\t${buffer}\tcoalesced\t$unroll\t${els_per_bank:-0}\t${comp}\t${type}\t$(gettime $file)" >> "times.txt"
    fi
}

if [[ $BASH_SOURCE == $0 ]]; then
    submit "$1" "$2" "$3" "$4" "${5:-}"
fi
