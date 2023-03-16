CONTIKI = ${PWD}/contiki-ng
#RELIC = ${PWD}/relic-0.5.0
RELIC = ${PWD}/relic
GMP =${PWD}/gmp


RELIC_CMAKE_SCRIPT ?= ${PWD}/relic-cmake-script.sh


#
# flags for the zolertia target
#
BOARD = remote-revb
PORT = /dev/ttyUSB0
TARGET = zoul

# RELIC_BUILD_DIR = ${PWD}/$(BUILD_DIR_TARGET)
RELIC_BUILD_DIR = ${PWD}/target


CONTIKI_PROJECT = main
PROJECTDIRS += src/ 
PROJECT_SOURCEFILES += stack-util.c getsp.s

ifdef FUNCTIONS
	ifneq (,$(findstring PYS,$(FUNCTIONS)))
		PROJECTDIRS+= src/pys/ 
		PROJECT_SOURCEFILES += pys.c pys_utils.c
		CFLAGS += -I $(PWD)/src/pys
	endif
	ifneq (,$(findstring BSW,$(FUNCTIONS)))
		PROJECTDIRS+= src/bsw/ 
		PROJECT_SOURCEFILES += bsw.c abe_utils.c
		CFLAGS += -I $(PWD)/src/bsw
	endif
	ifneq (,$(findstring GPSW,$(FUNCTIONS)))
		PROJECTDIRS+= src/gpsw/ 
		PROJECT_SOURCEFILES += gpsw.c abe_utils.c
		CFLAGS += -I $(PWD)/src/gpsw
	endif
	ifneq (,$(findstring ZH,$(FUNCTIONS)))
		PROJECTDIRS+= src/zh/ 
		PROJECT_SOURCEFILES += zh.c
		CFLAGS += -I $(PWD)/src/zh
	endif
endif
#
# add paths to relic for compiling and linking
#
CFLAGS += -I $(RELIC)/include -I $(RELIC_BUILD_DIR)/include  -I $(PWD)/src/include
#CFLAGS += -I $(GMP)/build  -I $(GMP)/build/include  -I $(GMP)/build/lib
#CFLAGS += -DCMAKE_PREFIX_PATH:PATH="$(GMP)/build"
PROJECT_LIBRARIES += $(RELIC_BUILD_DIR)/lib/librelic_s.a
#TARGET_LIBFLAGS +=  -lrelic_s -lgpsw
#DEFINES += PROJECT_CONF_H=\"../src/project-conf.h\"
#CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CFLAGS += -DPROJECT_CONF_PATH=\"project-conf.h\"

#
# Misc
#
# use all 32k of ram
CFLAGS += -DLPM_CONF_ENABLE=0
# https://stackoverflow.com/questions/13235748/linker-error-on-a-c-project-using-eclipse
LDFLAGS += -specs=nosys.specs 
# https://devzone.nordicsemi.com/f/nordic-q-a/1520/how-to-get-rid-of-impure_data
LDFLAGS += --specs=nano.specs
LDFLAGS += -ffunction-sections
#CFLAGS += -DCMAKE_PREFIX_PATH:PATH="/home/user/gcc-arm-none-eabi-9-2020-q2-update/build-native/host-libs/usr"

CONTIKI_NO_NET=1
MAKE_MAC =MAKE_MAC_NULLMAC
MAKE_NET = MAKE_NET_NULLNET
MAKE_ROUTING = MAKE_ROUTING_NULLROUTING
# no -Werror flag
WERROR = 0

CFLAGS+= -fconserve-stack

#
# Debugging stuff
#
# https://interrupt.memfault.com/blog/best-firmware-size-tools
LDFLAGS += -Wl,--print-memory-usage


ifdef DEBUG
	CFLAGS += -g -O0 -ggdb -DDEBUG
else
	CFLAGS += -pipe -std=c99 -O3 -funroll-loops -fomit-frame-pointer
#	CFLAGS += -pipe -std=c99 -Os 
#	CFLAGS += -pipe -std=c99 -O3 -fomit-frame-pointer
endif
#
# What to benchmark (space/time)
#
# https://stackoverflow.com/questions/6387614/how-to-determine-maximum-stack-usage-in-embedded-system-with-gcc
ifdef BENCH_SPACE
	CFLAGS += -fstack-usage -fdump-rtl-expand
	CFLAGS += -finstrument-functions
	CFLAGS += -DBENCH_SPACE
endif
ifdef BENCH_TIME
	CFLAGS += -DBENCH_TIME
endif

ifdef FUNCTIONS
	FFLAGS :=$(foreach wrd,$(FUNCTIONS),-D$(wrd))
	CFLAGS += $(FFLAGS)
else 
	CFLAGS += -DALL
endif

ifdef GPSW_SIM
	CFLAGS+= -DGPSW_SIM
endif

ifdef BSW_SIM
	CFLAGS+= -DBSW_SIM
endif

ifdef GPSW_N_ATTR
	CFLAGS+= -DGPSW_N_ATTR=$(GPSW_N_ATTR)
endif
ifdef GPSW_POLICY_STRING
	CFLAGS+= -DGPSW_POLICY_STRING=$(GPSW_POLICY_STRING)
endif

ifdef BSW_N_ATTR
	CFLAGS+= -DBSW_N_ATTR=$(BSW_N_ATTR)
endif
ifdef BSW_POLICY_STRING
	CFLAGS+= -DBSW_POLICY_STRING=$(BSW_POLICY_STRING)
endif
ifdef BSW_ATTRIBUTES
	CFLAGS+= -DBSW_ATTRIBUTES=$(BSW_ATTRIBUTES)
endif

ifdef ZH_N_ATTR
	CFLAGS+= -DZH_N_ATTR=$(ZH_N_ATTR)
endif
ifdef ZH_POLICY_STRING
	CFLAGS+= -DZH_POLICY_STRING=$(ZH_POLICY_STRING)
endif
ifdef ZH_USER_STRING
	CFLAGS+= -DZH_USER_STRING=$(ZH_USER_STRING)
endif
#DEFINES += PROJECT_CONF_PATH='"include/project-conf.h"'


SOURCE_LDSCRIPT = ${PWD}/cc2538.lds

.PHONY: librelic cclean relic_clean all

all: $(CONTIKI_PROJECT) 

#
# make rules for relic 
#
$(RELIC_BUILD_DIR)/lib/librelic_s.a: librelic

librelic: $(RELIC_BUILD_DIR)/Makefile
	$(Q)cd $(RELIC_BUILD_DIR) &&  make
	 
$(RELIC_BUILD_DIR)/Makefile: $(RELIC_CMAKE_SCRIPT)
	$(Q)mkdir -p $(RELIC_BUILD_DIR) && cd $(RELIC_BUILD_DIR) &&\
	CC="$(CC)" LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)" $(RELIC_CMAKE_SCRIPT) $(RELIC) 
	 

$(RELIC_BUILD_DIR)/lib/libgpsw.a: libgpsw

libgpsw : librelic
	$(CC) $(CFLAGS) $(LDFLAGS) -c src/gpsw/gpsw.c -o src/gpsw.o 
	arm-none-eabi-ar -rc $(RELIC_BUILD_DIR)/lib/libgpsw.a src/gpsw.o
	$(Q)$(RM) src/gpsw.o
#
# make rules for cleaning
#

$(RELIC_BUILD_DIR)/lib/libgpmp.a: libgmp

libgmp: 
	$(Q)cd $(GMP); mkdir -p build &&\
  	./configure --host=arm-none-eabi --build=kabylake-pc-linux-gnu  CFLAGS="-specs=nosys.specs" --disable-assembly ABI=32 \
		--prefix=$(GMP)/build &&\
  	make CFLAGS="-I /home/user/contiki-ng/relic/include -I /home/user/contiki-ng/target/include -I /home/user/contiki-ng/src/gpsw  -I /home/user/contiki-ng/src/include -I. -Isrc/ -I/home/user/contiki-ng/contiki-ng/arch/platform/zoul/. -I/home/user/contiki-ng/contiki-ng/arch/platform/zoul/dev -I/home/user/contiki-ng/contiki-ng/arch/platform/zoul/remote-revb -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/. -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/dev -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/usb -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/usb/common -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/usb/common/cdc-acm -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/../arm/cortex-m/cm3 -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/../arm/cortex-m -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/../arm/CMSIS/CMSIS/Core/Include -I/home/user/contiki-ng/contiki-ng/arch/cpu/cc2538/../arm/. -I/home/user/contiki-ng/contiki-ng/os -I/home/user/contiki-ng/contiki-ng/arch -I/home/user/contiki-ng/contiki-ng/os/sys -I/home/user/contiki-ng/contiki-ng/os/lib -I/home/user/contiki-ng/contiki-ng/os/dev -I/home/user/contiki-ng/contiki-ng/os/net -I/home/user/contiki-ng/contiki-ng/os/net/routing -I/home/user/contiki-ng/contiki-ng/os/net/mac -I/home/user/contiki-ng/contiki-ng/os/net/mac/framer -I/home/user/contiki-ng/contiki-ng/os/storage -I/home/user/contiki-ng/contiki-ng/os/services -I/home/user/contiki-ng/contiki-ng/os/lib/fs/fat -I/home/user/contiki-ng/contiki-ng/os/lib/fs/fat/option -I/home/user/contiki-ng/contiki-ng/arch/platform/zoul/fs/fat -I/home/user/contiki-ng/contiki-ng/arch/dev/storage/storage/disk/mmc -I/home/user/contiki-ng/contiki-ng/os/lib/newlib -I/home/user/contiki-ng/contiki-ng/os/lib/dbg-io -I/home/user/contiki-ng/contiki-ng/arch/dev/radio/cc1200 -I/home/user/contiki-ng/contiki-ng/arch/dev/etc/rgb-led -I/home/user/contiki-ng/contiki-ng/os/storage/cfs -I/home/user/contiki-ng/contiki-ng/os/net/mac/nullmac -I/home/user/contiki-ng/contiki-ng/os/net/nullnet -I/home/user/contiki-ng/contiki-ng/os/net/routing/nullrouting -I/home/user/contiki-ng/contiki-ng"
	make install
#	cp libgmp.a $(RELIC_BUILD_DIR)/lib/libgpmp.a

relic_clean:
	$(Q)$(RM) -r $(RELIC_BUILD_DIR)

gpsw_clean:
	$(Q)$(RM) $(RELIC_BUILD_DIR)/lib/libgpsw.a

gmp_clean:
	$(Q)cd $(GMP) && make distclean

cclean: distclean relic_clean gmp_clean
	$(Q)$(RM) *.su *.c.233r.expand




include $(CONTIKI)/Makefile.include
