#ifndef MUSIC2_PLUGIN_MAIN_H
#define MUSIC2_PLUGIN_MAIN_H
#include <plugin.h>

extern "C" {
    #include <music2_plugin_generated.h>
}

prx_thread * get_prx_main();

void __dso_handle() {
}

#define PRX_MAIN(x) prx_thread * get_prx_main() { static x prx; return &prx; }

extern "C" {
    int module_start(SceSize args, void *argp) {
        get_prx_main()->start(MAIN_THREAD, 3);
        sceKernelExitDeleteThread(0);
        return 0;
    }
    int module_reboot_before(SceSize args, void *argp) {
        get_prx_main()->stop();
        return 0;
    }
}

#endif // MUSIC2_PLUGIN_MAIN_H
