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
    this->arg[0] = this;
    this->arg[1] = arg;
    id = sceKernelCreateThread(thread_name, prx_thread_main, THREAD_PRIORITY+priority, 0x4000, 0, NULL);
   	sceKernelStartThread(id, 2, arg);
}

void prx_thread::stop() {
    sceKernelTerminateDeleteThread(id);
}

void prx_thread::start(const char * thread_name, int8_t priority) {
    start(thread_name, priority, nullptr);
}

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