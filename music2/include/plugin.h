#ifndef MUSIC2_PLUGIN_H
#define MUSIC2_PLUGIN_H

extern "C" {
    #include <pspkernel.h>
}

SceUID main_thid;

#define THREAD_PRIORITY 12
#define DELAY_THREAD_SEC 1000000

#include <music2_plugin_generated.h>

#endif // MUSIC2_PLUGIN_H
