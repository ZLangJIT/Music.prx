#ifndef MUSIC2_PLUGIN_MAIN_H
#define MUSIC2_PLUGIN_MAIN_H
#include <plugin.h>

extern "C" {
    #include <music2_plugin_generated.h>
}

prx_thread * get_prx_main();
SceUID prx_main_thread;

int prx_main_thread_fn(SceSize args, void *argp) {
    prx_thread p;
    p.run(NULL);
    //get_prx_main()->start(MAIN_THREAD, 3);
    //get_prx_main()->run(NULL);
    //get_prx_main()->stop();
    return 0;
}

#define PRX_MAIN(x) prx_thread * get_prx_main() { static x prx; return &prx; }

extern "C" {
    int module_start(SceSize args, void *argp) {
        prx_main_thread = sceKernelCreateThread("PRX_MAIN_THREAD_" MAIN_THREAD, prx_main_thread_fn, 12, 0x4000, 0, NULL);
       	sceKernelStartThread(prx_main_thread, args, argp);
        sceKernelExitDeleteThread(0);
        return 0;
    }
    int module_reboot_before(SceSize args, void *argp) {
        sceKernelTerminateDeleteThread(prx_main_thread);
        return 0;
    }

    __attribute__((visibility("hidden"))) void* __dso_handle = &__dso_handle;
}

#endif // MUSIC2_PLUGIN_MAIN_H
