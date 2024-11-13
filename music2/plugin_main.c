#include <pspkernel.h>
#include <pspmscm.h>

#include <music2_plugin_generated.h>

//main thread delay amt
//too high and the on screen display will flicker
//too low and the system will slow down
#define DELAY_THREAD_AMT 10000

#define DELAY_THREAD_SEC 1000000
#define THREAD_PRIORITY 12

SceUID main_thid;

void WaitMSReady() {
    while(MScmIsMediumInserted() <= 0)
        sceKernelDelayThreadCB(DELAY_THREAD_AMT);
    sceKernelDelayThreadCB(DELAY_THREAD_SEC);
}

int main_thread(SceSize args, void *argp)
{
    //wait until memory stick is readable
    WaitMSReady();
    
    // load up the display driver
    int ret;
    u32 uid = sceKernelLoadModule("ms0:/seplugins/music2/plugins/display.prx", 0, NULL);
    if (uid >= 0) {
        sceKernelStartModule(uid, 0, NULL, &ret, NULL);
    }

    while (1) {
        sceKernelDelayThreadCB(DELAY_THREAD_SEC*3);
    }
    return 0;
}

int module_start(SceSize args, void *argp) {
    main_thid = sceKernelCreateThread(MAIN_THREAD, main_thread, THREAD_PRIORITY+3, 0x4000, 0, NULL);
   	sceKernelStartThread(main_thid, args, argp);
    sceKernelExitDeleteThread(0);
    return 0;
}

int module_reboot_before(SceSize args, void *argp) {
    sceKernelTerminateDeleteThread(main_thid);
    return 0;
}
