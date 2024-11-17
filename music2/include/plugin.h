#ifndef MUSIC2_PLUGIN_H
#define MUSIC2_PLUGIN_H

#include <pspkernel.h>

//main thread delay amt
//too high and the on screen display will flicker
//too low and the system will slow down
#define DELAY_THREAD_AMT 10000

#define DELAY_THREAD_SEC 1000000
#define THREAD_PRIORITY 15

void waitMSReady();


#endif // MUSIC2_PLUGIN_H
