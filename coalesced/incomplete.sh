#!/bin/bash

incomplete=()

for i in *.aoco; do
    if [ ! -f ${i%o}x ]; then
	if [ ! -f ${i%o}x.tmp ]; then
	    incomplete+=( $i )
	fi
    fi
done

[ ${#incomplete[@]} -gt 0 ] && ls -ltd ${incomplete[@]%.aoco} || echo "None incomplete"
