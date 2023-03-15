#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=377
	#EP_PLAIN=off
	#EP_SUPER=off
	EP_PRECO=off #occupy to much space!
	#MULTI=PTHREAD
	#EP_WIDTH=2
	#EP_CTMAP=off
	#FP_QNRES=off
	BN_PRECI=512
	#STRIP=on
	#FPX_METHD="INTEG;INTEG;BASIC" # lazy reduction not implemented!
	#FPX_METHD="BASIC;BASIC;BASIC"
	#BN_METHD="COMBA;COMBA;BASIC;BASIC;BASIC;BASIC"
)
echo ${cmake_args[*]}

CC=$CC CXX=$CXX LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1