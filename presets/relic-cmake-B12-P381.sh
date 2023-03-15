#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=381
	FP_QNRES=on
	EP_PRECO=off #occupy to much space!
	BN_PRECI=512
	FPX_METHD="INTEG;INTEG;BASIC" # lazy reduction not implemented!
	#FPX_METHD="BASIC;BASIC;BASIC"
)
echo ${cmake_args[*]}

CC=$CC CXX=$CXX LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1