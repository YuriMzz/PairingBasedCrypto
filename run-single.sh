#make relic_clean
#make BENCH_TIME=1 FUNCTIONS=GPSW  GPSW_SIM=1 RELIC_CMAKE_SCRIPT=/home/user/contiki-ng//presets/relic-cmake-BN-P158.sh
#make BENCH_TIME=1  FUNCTIONS=GPSW  GPSW_SIM=1 GSPW_N_ATTR=2 GPSW_POLICY_STRING='"1 0 2of2"' main.upload login

#make librelic RELIC_CMAKE_SCRIPT=/home/user/contiki-ng//presets/relic-cmake-BN-P158.sh
make librelic TARGET=zoul BENCH_TIME=1 FUNCTIONS=GPSW \
RELIC_CMAKE_SCRIPT=/home/user/contiki-ng//presets/relic-cmake-BN-P158.sh \
main.upload login
#make BENCH_TIME=1  FUNCTIONS=GPSW  GPSW_SIM=1 GSPW_N_ATTR=2 GPSW_POLICY_STRING='"1 0 2of2"' 
