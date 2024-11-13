#include <plugin_main.h>
#include "blit.h"

int main_thread() {
    char str_buf[120+1];
    int counter = 0;
    while(1) {
        counter++;
        sprintf(str_buf, "%d blit%s...", counter, counter == 1 ? "" : "s");
        blit_string(0, 20, str_buf, 0xffffff, 0x000000);
        sceKernelDelayThreadCB(DELAY_THREAD_AMT/2);
    }
}
