#ifndef MUSIC2_PLUGIN_MAIN_H
#define MUSIC2_PLUGIN_MAIN_H
#include <plugin.h>

extern "C" {
    #include <music2_plugin_generated.h>
}

#define PRX_MAIN(x) prx_thread * get_prx_main() { static x prx; return &prx; }

prx_thread * get_prx_main();

extern "C" {

    int module_start(SceSize args, void *argp) {
        get_prx_main()->start(THREAD_NAME, THREAD_PRIORITY+3, NULL);
        sceKernelExitDeleteThread(0);
        return 0;
    }
    
    int module_reboot_before(SceSize args, void *argp) {
        get_prx_main()->stop();
        return 0;
    }

    __attribute__((visibility("hidden")))
    void* __dso_handle = &__dso_handle;
}
#endif // MUSIC2_PLUGIN_MAIN_H
