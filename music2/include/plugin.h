#ifndef MUSIC2_PLUGIN_H
#define MUSIC2_PLUGIN_H

extern "C" {
    #include <pspkernel.h>
}

//main thread delay amt
//too high and the on screen display will flicker
//too low and the system will slow down
#define DELAY_THREAD_AMT 10000

#define DELAY_THREAD_SEC 1000000
#define THREAD_PRIORITY 12

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
