#ifndef MUSIC2_PLUGIN_H
#define MUSIC2_PLUGIN_H

#include <pspkernel.h>
#include <pspmscm.h>

//main thread delay amt
//too high and the on screen display will flicker
//too low and the system will slow down
#define DELAY_THREAD_AMT 10000

#define DELAY_THREAD_SEC 1000000
#define THREAD_PRIORITY 15

#define waitMSReady() { while(MScmIsMediumInserted() <= 0) sceKernelDelayThreadCB(DELAY_THREAD_AMT); sceKernelDelayThreadCB(DELAY_THREAD_SEC); }

#endif // MUSIC2_PLUGIN_H
