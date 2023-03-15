#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=158
	BN_PRECI=256
	EP_PRECO=off #occupy to much space!
	#FPX_METHD="BASIC;BASIC;BASIC"
	#EP_METHD="BASIC;BASIC;COMBS;INTER"
	STRIP=off
	#ARITH=gmp 
	#FP_QNRES=off
)
echo ${cmake_args[*]}

CC=$CC CXX=c++ LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1