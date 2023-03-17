# PairingBasedCrypto

This repository contains some library function implememnting pairing based cryptography using RELIC library function for the required math. 
It also contains some utility function for building cryptographic schemes, benchmarking and debugging.
main.c is meant to be used with contiki-NG 

Tested with a custom version of RELIC library available at: https://github.com/YuriMzz/relic/tree/yuri_dbg

The Makefile is used to statically compile RELIC, than link it with contiki. An attempt was made to compile the library with GMP (https://github.com/wellsaid/gmp), but it was not possile with the target architecture (Zolertia REmote).

Input data for the cryptographic function have to be given using compilation variables. 

Directory raw contains data from experiments along with some plots
