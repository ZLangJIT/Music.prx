TARGET = music
OBJS = main.o

PSP_FW_VERSION = 371
BUILD_PRX = 1

USE_KERNEL_LIBS = 1

PRX_EXPORTS = exports.exp

INCDIR = ./include
CFLAGS = -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./lib
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
