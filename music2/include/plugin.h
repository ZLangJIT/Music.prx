#ifndef MUSIC2_PLUGIN_H
#define MUSIC2_PLUGIN_H

extern "C" {
    #include <pspkernel.h>
}

#define DELAY_THREAD_SEC 1000000

class prx_thread {
    SceUID id;
    void * arg[2];
    public:
    void start(const char * thread_name, int8_t priority);
    void start(const char * thread_name, int8_t priority, void * arg);
    void stop();
    inline virtual void run(void * arg) {
        while(1)
          sceKernelDelayThreadCB(DELAY_THREAD_SEC*1);
    };
    inline virtual ~prx_thread() {};
};

#endif // MUSIC2_PLUGIN_H
