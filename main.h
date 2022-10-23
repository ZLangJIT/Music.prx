#ifndef MAIN__H__
#define MAIN__H__

//tp 8
//tp 12
#define THREAD_PRIORITY 12

#define VERSION 0x0500
#define OMG_AUDIO_DIR "ms0:/OMGAUDIO/"
#define DISPLAY_TIMER_AMT 65

#define SPEED_NUM 9
#define SPEED_NEXT 1
#define SPEED_PREV (-1)

#define TYPE_AT3 1
#define TYPE_MP3 2
#define TYPE_UNK -1

int main_thread(SceSize args, void *argp);
int module_start(SceSize args, void *argp); //handles button input
int module_reboot_before(SceSize args, void *argp);

void WaitMSReady();
int display_thread(SceSize args, void *argp); //handles on screen display
int playlist_thread(SceSize args, void *argp);//handles the playlist

void ResetPlaylist();
void InitPlaylist();
void TerminatePlaylist();
int CountMusicFiles(char* dir);
int GetMusicFileName(char* dir,char* filename,int num,int basenum);

SceUID LoadStartModule(char *modname, int partition);
void PowerCallback(int unknown, int powerInfo);

int cpu_thread(SceSize args, void *argp);

u32 GetNextSpeedNum(int dir);

void GetOMGTitle(char *fname, char *title);
char GetOMGFileType(char *fname);
int GetID3TagSize(char *fname);

extern u32 cpu_speed[2];
extern char set_cpu_speed;
#endif
