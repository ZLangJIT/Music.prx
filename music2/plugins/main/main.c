#include <plugin_main.h>

int main_thread() {
    //wait until memory stick is readable
    waitMSReady();
    
    // load up the display driver
    u32 uid = sceKernelLoadModule("ms0:/seplugins/music2/plugins/display.prx", 0, NULL);
    if (uid >= 0) {
        int ret;
        sceKernelStartModule(uid, 0, NULL, &ret, NULL);
    }

    // while (1) {
    //     sceKernelDelayThreadCB(DELAY_THREAD_SEC*3);
    // }
    return 0;
}
