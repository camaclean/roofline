#!/bin/bash

set -eu

skip_aoco=

declare -A typemap=( [float]=f [double]=d [int]=i )

readarray -t completed < submitted.txt

firststage_only=

submit() {
    local i buffer type unroll comp name els_per_bank
    buffer=$1
    type=$2
    unroll=$3
    comp=$4
    els_per_bank=${5:-}
    export buffer type unroll comp firststage_only els_per_bank
    [ -z "${els_per_bank}" ] && name="swi_${buffer}_coalesced_${unroll}_${comp}_${type}" || name="swi_${buffer}_coalesced_${unroll}_${els_per_bank}_${comp}_${type}"
    [ -z "${els_per_bank}" ] && shortname="s_${buffer}_c_${unroll}_${comp}_${typemap[$type]}" || shortname="s_${buffer}_c_${unroll}_${els_per_bank}_${comp}_${typemap[$type]}"

    [ -f "$name.aocx" ] && return
    [ ! -z "$skip_aoco" ] && [ -f "$name.aoco" ] && return
    for i in "${completed[@]}"; do
	[[ "$i" == $shortname ]] && return
    done
    #echo $els_per_bank
    #echo qsub -V -N "$shortname" swi.pbs
    qsub -V -N "$shortname" swi.pbs
    echo "$shortname" >> submitted.txt
}

#submit 2 float 1   1
#submit 2 float 1   8
#submit 2 float 1   128
#submit 2 float 1   256
#submit 2 float 1   512
#submit 2 float 1   1024
#submit 2 float 1   1515
#submit 2 float 2   1
#submit 2 float 2   757
#submit 2 float 4   1
#submit 2 float 4   377
#submit 2 float 8   1
#submit 2 float 8   128
#submit 2 float 8   186
#submit 2 float 16  1
#submit 2 float 16  92
#submit 2 float 32  1
#submit 2 float 32  45
#submit 2 float 64  1
#submit 2 float 64  21
#submit 2 float 128 1
#submit 2 float 128 9
submit 2 float 192 1

#submit 2 double 1   1
#submit 2 double 1   8
#submit 2 double 1   79
#submit 2 double 2   39
#submit 2 double 4   1
#submit 2 double 4   19
#submit 2 double 8   1
#submit 2 double 8   10

submit local float 1 1 1
submit local float 1 8 1
submit local float 1 64 1
submit local float 1 128 1 
submit local float 1 256 1
submit local float 1 512 1
submit local float 1 1024 1
submit local float 1 1515 1
submit local float 1 3030 1
submit local float 2 1 1
submit local float 2 1 2
submit local float 2 757 1
submit local float 2 757 2
submit local float 2 1514 2
submit local float 2 1515 2
submit local float 2 1516 2
submit local float 4 1 1
submit local float 4 1 2
submit local float 4 1 4
submit local float 4 377 1
submit local float 4 377 2
submit local float 4 377 4
submit local float 4 754 4
submit local float 4 757 4
submit local float 4 758 4
submit local float 8 1 1 
submit local float 8 1 2 
submit local float 8 1 4 
submit local float 8 1 8 
submit local float 8 128 1
submit local float 8 128 2
submit local float 8 128 4
submit local float 8 128 8
submit local float 8 186 1
submit local float 8 186 2
submit local float 8 186 4
submit local float 8 186 8
submit local float 8 377 8
submit local float 8 379 8
submit local float 16 1 1
submit local float 16 1 2
submit local float 16 1 4
submit local float 16 1 8
submit local float 16 1 16
submit local float 16 64 1
submit local float 16 64 2
submit local float 16 64 4
submit local float 16 64 8
submit local float 16 64 16
submit local float 16 94 16
submit local float 16 186 16
submit local float 16 189 16
submit local float 32 1 1
submit local float 32 1 2
submit local float 32 1 4
submit local float 32 1 8
submit local float 32 1 16
submit local float 32 1 32
submit local float 32 45 1
submit local float 32 45 2
submit local float 32 45 4
submit local float 32 45 8
submit local float 32 45 16
submit local float 32 45 32
submit local float 32 94 32
submit local float 64 1 1
submit local float 64 1 2
submit local float 64 1 4
submit local float 64 1 8
submit local float 64 1 16
submit local float 64 1 32
submit local float 64 1 64
submit local float 64 21 1
submit local float 64 21 2
submit local float 64 21 4
submit local float 64 21 8
submit local float 64 21 16
submit local float 64 21 32
submit local float 64 21 64
submit local float 64 47 64
submit local float 128 1 1
submit local float 128 1 2
submit local float 128 1 4
submit local float 128 1 8
submit local float 128 1 16
submit local float 128 1 32
submit local float 128 1 64
submit local float 128 1 128
submit local float 128 9 1
submit local float 128 9 2
submit local float 128 9 4
submit local float 128 9 8
submit local float 128 9 16
submit local float 128 9 32
submit local float 128 9 64
submit local float 128 9 128
submit local float 128 20 1
submit local float 128 20 128
submit local float 128 23 128
submit local float 256 1 1
submit local float 256 1 2
submit local float 256 1 4
submit local float 256 1 8
submit local float 256 1 16
submit local float 256 1 32
submit local float 256 1 64
submit local float 256 1 128
submit local float 256 1 256
submit local float 256 4 1
submit local float 256 4 256
submit local float 256 11 256
submit local float 512 1 1
submit local float 512 1 2
submit local float 512 1 4
submit local float 512 1 8
submit local float 512 1 16
submit local float 512 1 32
submit local float 512 1 64
submit local float 512 1 128
submit local float 512 1 256
submit local float 512 1 512
submit local float 512 5 512
