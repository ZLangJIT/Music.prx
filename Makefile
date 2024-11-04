TARGET = music
OBJS = main.o \
       blit.o \
       config.o \
       font.o \
       sceAudio_driver.o \
       sceUtility.o \
       sceDisplay_driver.o \
       scePower_driver.o \
       sceCtrl_driver.o \
       LoadCoreForKernel.o \
       init.o \
       hw.o \
       hw_mp3.o \
       hw_at3.o \
       hw_aa3.o \
       exports.o

PSP_FW_VERSION = 371
BUILD_PRX = 1

USE_KERNEL_LIBS = 1

PRX_EXPORTS = exports.exp

INCDIR = ./include
CFLAGS = -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic
#CFLAGS = -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-builtin-printf
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./lib
LIBS = -lpspaudiocodec -lpspaudio -lpspkernel -lm -lpsplibc -lpspsystemctrl_kernel -lpspumd

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
