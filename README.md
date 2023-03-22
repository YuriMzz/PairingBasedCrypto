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
* I also provide the bash script i used to start it setting the right privileges and mounting volumes (`run-docker.sh`)

## Compiling
* `Makefile` is used to statically compile RELIC, than link it with contiki. An attempt was made to compile the library with GMP (https://github.com/wellsaid/gmp), but it was not possile with the target architecture (Zolertia REmote).
* in order to compile you have to:
  * clone contiki-NG and RELIC inside the root folder
  * start docker container
  * use the scripts to compile and flash into the target platform

## Usage
* `make librelic TARGET=zoul main.upload login` is the command to compile and execute
* Inside the command is possible to define envinroment variables, wich are used during compilation ans execution
  * `RELIC_CMAKE_SCRIPT` defines the file containing RELIC configuration for the specific curve
  * `BENCH_TIME` if present will configure the compilation for measuring the execution time of each function
  * `BENCH_SPACE` if present will configure the compilation for measuring the stack used during execution
    * this configuration will negatively impact execution time so it's not possible to mesure TIME and SPACE in the same run
  * `FUNCTIONS` its a space separated string wich represent the list of schemes to be tested; conditional compilation is done in order to minimize binary size.
    * available schemes are: IBE BBS BBS2 BLS CLS ZSS IBE ZH GPSW BSW. PYS scheme implementation is incomplete.
* IBE and Short Signature schemes generate random input for benchmarking inside the binary, while input data for ABE cryptographic function have to be given using envinroment variables. 
* `run-single.sh` is useful for dedugging a specific input

## Batch runs
* In order to run repetitive runs and collect data i made two scripts; they don't take command line parameters, but one can change the variables in the code in order to change the execution
  * `run-batch.py` is used for IBE and Short Signatures schemes
    * this script only take care to execute the program 2 times (TIME and SPACE) for each curve in the array CURVES and colllect the results in the appropriate folder (repetition are done inside the program)
  * `run-<scheme>-batch.py` are used for other schemes 
    * those scripts are able to generate different input parameters for each run and they will perform a variable number of repetition specified with the variable REPETITIONS; 
    * they can also automatically test different number of attributes (contained in the array ATTRIBUTES)
    * each scheme have a different format of input parameters, and dedicated function to generate them randomly (average and worst scenario)
    * scenario and implementation type (for GPSW and BSW) are fixed and have to be changed manually
* `power_meter.py` is used to obtain data from the AVHzY CT-2 power meter. This tool was insiperd by https://github.com/wellsaid/Py-AVHzY-CT-2
  * the script is called inside `run-<>-batch.py` scripts; this is an attempt to obtain the maximum available precision out of the instrument, but it turns out to be quite useless, because the poewr consumpion is resulted to be proportional to the execution time.

## Analytics
* Directory `raw` contains data from experiments along with some plots
* Plots are generated using python scripts under the directory `tools`
  * `plots.py` is used for IBE and Short Signatures schemes
  * `<scheme>-plots.py` are used for other schemes
