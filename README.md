# PairingBasedCrypto

* This repository contains some library function implememnting pairing based cryptography using RELIC library function for the required math. 
* It also contains some utility function for building cryptographic schemes, benchmarking and debugging.
* `main.c` is meant to be used with contiki-NG 

* Tested with a custom version of RELIC library available at: https://github.com/YuriMzz/relic/tree/yuri_dbg
  * diff against the original version are related to integration of the logging system with contiki-NG and to the optimization wrt target platform
  
* Tested with this version of contiki-NG: https://github.com/contiki-ng/contiki-ng/tree/95037980e21180833cc73fe17371f38e732e5206
  * probably newer versions will work fine as well

## Envinroment
* I used contiker https://docs.contiki-ng.org/en/develop/doc/getting-started/Docker.html on an Arch Linux host system.
* I provide a Dockerfile for building
* I also provide the bash script i used to start it setting the right privileges and mounting volumes

## Compiling
* `Makefile` is used to statically compile RELIC, than link it with contiki. An attempt was made to compile the library with GMP (https://github.com/wellsaid/gmp), but it was not possile with the target architecture (Zolertia REmote).
* in order to compile you have to:
  * clone contiki-NG and RELIC inside the root folder
  * start docker container
  * use the scripts to compile and flash into the target platform


## Usage
* Input data for the cryptographic function have to be given using compilation variables. 
* I provide a copy of bash and python scripts used to run tests. 
  * `run-single.sh` is useful for dedugging a specific input
  * `run-batch.py` is used for IBE and Short Signatures schemes
  * `run-<scheme>-batch.py` are used for other schemes
* `power_meter.py` is used to obtain data from the AVHzY CT-2 power meter. This tool was insiperd by https://github.com/wellsaid/Py-AVHzY-CT-2

## Analytics
* Directory `raw` contains data from experiments along with some plots
* Plots are generated using python scripts under the directory `tools`
  * `plots.py` is used for IBE and Short Signatures schemes
  * `<scheme>-plots.py` are used for other schemes
