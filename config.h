#ifndef CONFIG__H__
#define CONFIG__H__

#define MUSIC_DIR "ms0:/PSP/MUSIC/"
#define MAX_CONTROL_OPTIONS 12


enum {
OPT_VOLUP,
OPT_VOLDOWN,
OPT_START_STOP,
OPT_PREVIOUS,
OPT_NEXT,
OPT_MODE_TOGGLE,
OPT_CPU_NEXT,
OPT_CPU_PREV,
OPT_RELOAD,
OPT_DISPLAY,
OPT_IN_GAME_MUTE,
OPT_LOOP,
};

typedef struct{
u32 control[MAX_CONTROL_OPTIONS];
char found;
char debug;
char oc_wlan;
char vsh_clock;
char dirname[262];//maxpath is 261 (not 256) with ms0:/ +1 for '\0' 
}MusicConf;

void LoadConfigFile(char *fname, MusicConf *config);
#endif
