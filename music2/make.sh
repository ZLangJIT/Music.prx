psp-g++ -I /usr/local/pspdev/psp/sdk/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -fno-exceptions -fno-rtti -c -o main.o main.cpp

psp-g++ -D_PSP_FW_VERSION=371 -L./lib -L/usr/local/pspdev/psp/sdk/lib -Wl,-q,-T/usr/local/pspdev/psp/sdk/lib/linkfile.prx -nostartfiles *.o -o music2.elf

psp-fixup-imports music2.elf
psp-prxgen music2.elf music2.prx
