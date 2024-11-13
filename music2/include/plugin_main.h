#ifndef MUSIC2_PLUGIN_MAIN_H
#define MUSIC2_PLUGIN_MAIN_H
#include <plugin.h>
#include <music2_plugin_generated.h>

SceUID m2prx_main_thid;

int main_thread();

int m2prx_main_thread(SceSize args, void *argp) {
    return main_thread();
}

int module_start(SceSize args, void *argp) {
    m2prx_main_thid = sceKernelCreateThread(MAIN_THREAD, m2prx_main_thread, THREAD_PRIORITY+3, 0x4000, 0, NULL);
   	sceKernelStartThread(m2prx_main_thid, args, argp);
    sceKernelExitDeleteThread(0);
    return 0;
}

int module_stop(SceSize args, void *argp) {
    sceKernelTerminateDeleteThread(m2prx_main_thid);
    return 0;
}

int module_reboot_before(SceSize args, void *argp) {
    sceKernelTerminateDeleteThread(m2prx_main_thid);
    return 0;
}

#endif // MUSIC2_PLUGIN_MAIN_H
