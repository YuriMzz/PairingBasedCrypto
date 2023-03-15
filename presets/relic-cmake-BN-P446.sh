#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=446
	EP_PRECO=off #occupy to much space!
	#STRIP=on
	BN_PRECI=512
	#AMALG=off
	FPX_METHD="INTEG;INTEG;BASIC" # lazy reduction not implemented!
)
echo ${cmake_args[*]}

CC=$CC CXX=$CXX LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1