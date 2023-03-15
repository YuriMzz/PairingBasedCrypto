#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=509
	EP_PLAIN=off
	EP_SUPER=off
	EP_PRECO=off #occupy to much space!
	ED_PRECO=off
	FP_PRECO=off
	#MULTI=PTHREAD
	#EP_WIDTH=2
	#EP_CTMAP=off
	#FP_QNRES=off
	BN_PRECI=2048
	STRIP=on
	FP_PMERS=off 
	#FP_QNRES=on 
	#FPX_METHD="INTEG;INTEG;LAZYR" 
	#PP_METHD="LAZYR;OATEP"
	#WITH="ALL"
	#FPX_METHD="INTEG;INTEG;LAZYR" # lazy reduction not implemented!
	EP_METHD="PROJC;BASIC;COMBD;TRICK"
	#EP_METHD="PROJC;BASIC;COMBD;INTER"
	FPX_METHD="BASIC;BASIC;BASIC"
	#BN_METHD="COMBA;COMBA;BASIC;BASIC;BASIC;BASIC"
	ALLOC=AUTO
)
echo ${cmake_args[*]}

CC=$CC CXX=$CXX LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1