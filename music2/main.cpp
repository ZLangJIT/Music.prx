#include <plugin_main.h>
struct main_thread;
PRX_MAIN(main_thread);
struct main_thread : public prx_thread {
    void run(void * arg) override {
        while(1)
          sceKernelDelayThreadCB(DELAY_THREAD_SEC*1);
    }
};
