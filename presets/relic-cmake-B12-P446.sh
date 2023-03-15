#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=446
	FP_QNRES=on
	EP_PRECO=off
	#EP_DEPTH=2
	BN_PRECI=512
	FPX_METHD="INTEG;INTEG;BASIC" # lazy reduction not implemented!
)
echo ${cmake_args[*]}

CC=$CC CXX=$CXX LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1