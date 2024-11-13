#include <plugin.h>

#define THREAD_PRIORITY 12

int prx_thread_main(SceSize args, void *argp) {
    reinterpret_cast<prx_thread*>(
        reinterpret_cast<void**>(argp)[0]
    )->run(
        reinterpret_cast<void**>(argp)[1]
    );
    return 0;
}

void prx_thread::start(const char * thread_name, int8_t priority, void * arg) {
    arg[0] = this;
    arg[1] = arg;
    id = sceKernelCreateThread(thread_name, prx_thread_main, THREAD_PRIORITY+priority, 0x4000, 0, NULL);
   	sceKernelStartThread(id, 1, arg);
}

void prx_thread::stop(const char * thread_name) {
    sceKernelTerminateDeleteThread(id);
}

void prx_thread::start(const char * thread_name, int8_t priority) {
    start(thread_name, priority, nullptr);
}
