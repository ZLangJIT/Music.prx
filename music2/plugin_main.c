#include <pspkernel.h>
#include <pspmscm.h>

#include <music2_plugin_generated.h>

//main thread delay amt
//too high and the on screen display will flicker
//too low and the system will slow down
#define DELAY_THREAD_AMT 10000

#define DELAY_THREAD_SEC 1000000
#define THREAD_PRIORITY 12

//void * get_prx_main();

SceUID main_thid;

void WaitMSReady()//wait until ms is ready
{
	int ret;
    ret = MScmIsMediumInserted();
    while(ret <= 0)
    {
        sceKernelDelayThreadCB(DELAY_THREAD_AMT);
        ret = MScmIsMediumInserted();
    }
    sceKernelDelayThreadCB(DELAY_THREAD_SEC);
}

int main_thread(SceSize args, void *argp)
{
    //wait until memory stick is readable
    WaitMSReady();

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

__attribute__((visibility("hidden")))
void* __dso_handle = &__dso_handle;
