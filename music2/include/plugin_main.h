#ifndef MUSIC2_PLUGIN_MAIN_H
#define MUSIC2_PLUGIN_MAIN_H
#include <plugin.h>

extern "C" {
    #include <music2_plugin_generated.h>
}

#define PRX_MAIN(x) prx_thread * get_prx_main() { static x prx; return &prx; }

#endif // MUSIC2_PLUGIN_MAIN_H
