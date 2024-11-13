#include <plugin_main.h>
struct display_thread : public prx_thread {
    void run(void * arg) override {
        while(1)
          sceKernelDelayThreadCB(DELAY_THREAD_SEC*1);
    }
};
PRX_MAIN(display_thread);
