extern "C" {
    #include <pspkernel.h>
}

SceUID main_thid;

#define THREAD_PRIORITY 12
#define DELAY_THREAD_SEC 1000000

//main thread delay amt
//too high and the on screen display will flicker
//too low and the system will slow down
#define DELAY_THREAD_AMT 10000

int main_thread(SceSize args, void *argp) {
    while(1) {
        sceKernelDelayThreadCB(DELAY_THREAD_SEC*1);
    }
    return 0;
}

extern "C" {
    //handles button input
    int module_start(SceSize args, void *argp) {
        main_thid = sceKernelCreateThread("music2_main", main_thread, THREAD_PRIORITY+3, 0x4000, 0, NULL);
       	sceKernelStartThread(main_thid, args, argp);
        sceKernelExitDeleteThread(0);
        return 0;
    }
    int module_reboot_before(SceSize args, void *argp) {
        sceKernelTerminateDeleteThread(main_thid);
        //TerminatePlaylist();
        return 0;
    }
    PSP_MODULE_INFO("Music2_prx", 0x1000, 1, 1);
}

