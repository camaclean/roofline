#!/bin/bash

set -eu

csv=

if [ ! -z "${1:-}" ]; then
    csv="yes"
fi

for f in *.aocx; do
    if [[ $f =~ (swi|ndrange)_([0-9]+)_(queued|coalesced)_([0-9]+)_([0-9]+)_([a-z_]+)\.aocx ]]; then
        ktype=${BASH_REMATCH[1]}
        buffers=${BASH_REMATCH[2]}
        qtype=${BASH_REMATCH[3]}
        unroll=${BASH_REMATCH[4]}
        ert_ops=${BASH_REMATCH[5]}
        dtype=${BASH_REMATCH[6]}
    fi
    dsp_str=$( grep "DSP blocks"  --binary-files=text $f )
    lut_str=$( grep "Logic utilization"  --binary-files=text $f )
    if [[ $dsp_str =~ .*[[:space:]]([0-9,\.]+)[[:space:]]/[[:space:]]([0-9,\.]+).* ]]; then
        dsp_used=${BASH_REMATCH[1]//,/}
        dsp_max=${BASH_REMATCH[2]//,/}
    fi
    if [[ $lut_str =~ .*[[:space:]]([0-9,\.]+)[[:space:]]/[[:space:]]([0-9,\.]+).* ]]; then
        lut_used=${BASH_REMATCH[1]//,/}
        lut_max=${BASH_REMATCH[2]//,/}
    fi
    if [ -z "$csv" ]; then
        echo $ktype $buffers $qtype $unroll $ert_ops $dtype $dsp_used $dsp_max $lut_used $lut_max
    else
        echo $ktype,$buffers,$qtype,$unroll,$ert_ops,${dtype//_/ },$dsp_used,$dsp_max,$lut_used,$lut_max
    fi
done
