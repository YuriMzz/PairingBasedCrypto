#!/bin/bash

. ${PWD}/../relic_cmake_common_args.sh

cmake_args=(${cmake_args[*]}
	FP_PRIME=254
	EP_PRECO=off
	#EP_DEPTH=3
	EP_CTMAP=off
	STRIP=off
)
echo ${cmake_args[*]}

CC=$CC CXX=$CXX LINK=$LDFLAGS COMP=$CFLAGS cmake -D$(echo "${cmake_args[*]}" | sed 's/ / -D/g') $1