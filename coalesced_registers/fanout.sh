#!/bin/bash

set -e
#    [ -z "${els_per_bank}" ] && name="swi_${buffer}_coalesced_${unroll}_${comp}_${type}" || name="swi_${buffer}_coalesced_${unroll}_${els_per_bank}_${comp}_${type}"
parse_name()
{
    local kerntype buffers queuetype unroll flops els_per_bank dtype
    if [[ $1 =~ (ndrange|swi)_([0-9]+)_(coalesced|queued)_([0-9]+)_([0-9]+)_(.*).aocx ]]; then
	kerntype=${BASH_REMATCH[1]}
	buffers=${BASH_REMATCH[2]}
	queuetype=${BASH_REMATCH[3]}
	unroll=${BASH_REMATCH[4]}
	flops=${BASH_REMATCH[5]}
	els_per_bank=0
	dtype=${BASH_REMATCH[6]}
        echo $kerntype,$buffers,$queuetype,$unroll,$flops,$els_per_bank,$dtype
    fi
    if [[ $aocx =~ (ndrange|swi)_local_(coalesced|queued)_([0-9]+)_([0-9]+)_([0-9]+)_(.*).aocx ]]; then
	kerntype=${BASH_REMATCH[1]}
	buffers=0
	queuetype=${BASH_REMATCH[2]}
	unroll=${BASH_REMATCH[3]}
	flops=${BASH_REMATCH[4]}
	els_per_bank=${BASH_REMATCH[5]}
	dtype=${BASH_REMATCH[6]}
        echo $kerntype,$buffers,$queuetype,$unroll,$flops,$els_per_bank,$dtype
    fi
}
defaultfile="${0%.sh}.csv"
outfile=${1:-${defaultfile}}
header="Kernel Type, Buffers, Queue Type, Unroll, ERT FLOPs, Elements per Bank, Data Type, Highest Non-Global Fanout"
echo $header >$outfile
echo $header >/dev/stderr
for aocx in *.aocx; do
    freq=$(strings $aocx | grep -oP 'Highest non-global fanout: \K\w+')
    echo $(parse_name $aocx),$freq >>$outfile
    echo $(parse_name $aocx),$freq >>/dev/stderr
done
