#!/bin/bash

set -eo

for f in *.aocx; do
    #swi_2_coalesced_16_2_long.aocx

    echo $f
    if [[ $f =~ (swi|ndrange)_([0-9]+)_(queued|coalesced)_([0-9]+)_([0-9]+)_([a-z]+)\.aocx ]]; then
        ktype=${BASH_REMATCH[1]}
        buffers=${BASH_REMATCH[2]}
        qtype=${BASH_REMATCH[3]}
        unroll=${BASH_REMATCH[4]}
        ert_ops=${BASH_REMATCH[5]}
        dtype=${BASH_REMATCH[6]}
    fi
    if [[ "$f" == *float*  || "$f" == *double* ]]; then
        str=$( grep "DSP blocks"  --binary-files=text $f )
        dsp="yes"
    else
        str=$( grep "Logic utilization"  --binary-files=text $f )
        dsp=
    fi
    echo $str
    if [[ $str =~ .*[[:space:]]([0-9,\.]+)[[:space:]]/[[:space:]]([0-9,\.]+).* ]]; then
        used=${BASH_REMATCH[1]//,/}
        max=${BASH_REMATCH[2]//,/}
        if [ ! -z "$dsp" ]; then
            echo ${BASH_REMATCH[1]//,/}
            if [ ${BASH_REMATCH[1]//,/} -eq 0 ]; then
                str=$( grep "Logic utilization"  --binary-files=text $f )
                if [[ $str =~ .*[[:space:]]([0-9,\.]+)[[:space:]]/[[:space:]]([0-9,\.]+).* ]]; then
                    echo Alternate: $str
                    used=${BASH_REMATCH[1]//,/}
                    max=${BASH_REMATCH[2]//,/}
                fi
            fi
        fi
    fi
    echo $ktype $buffers $qtype $unroll $ert_ops $dtype $used $max
done
