#include <pspuser.h>
#include <psppower.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <string.h>
#include <stdlib.h>

#include "playback.h"
#include "main.h"
#include "config.h"
#include "log.h"


char ControlOptions[MAX_CONTROL_OPTIONS][30] = {
"VOLUP",
"VOLDOWN",
"START_STOP",
"PREVIOUS",
"NEXT",
"MODE_TOGGLE",
"CPU_NEXT",
"CPU_PREV",
"RELOAD",
"DISPLAY",
"IN_GAME_MUTE",
"LOOP",
};

u32 DefaultControl[MAX_CONTROL_OPTIONS] = {
PSP_CTRL_NOTE|PSP_CTRL_UP,//VOLUP
PSP_CTRL_NOTE|PSP_CTRL_DOWN,//VOLDOWN
PSP_CTRL_NOTE|PSP_CTRL_LTRIGGER,//START_STOP
PSP_CTRL_NOTE|PSP_CTRL_LEFT,//PREVIOUS
PSP_CTRL_NOTE|PSP_CTRL_RIGHT,//NEXT
PSP_CTRL_NOTE|PSP_CTRL_RTRIGGER,//MODE_TOGGLE
PSP_CTRL_NOTE|PSP_CTRL_TRIANGLE,//CPU_NEXT
PSP_CTRL_NOTE|PSP_CTRL_CROSS,//CPU_PREV
PSP_CTRL_NOTE|PSP_CTRL_CIRCLE,//RELOAD
PSP_CTRL_NOTE|PSP_CTRL_SQUARE,//DISPLAY
PSP_CTRL_NOTE|PSP_CTRL_SELECT,//In Game Mute
PSP_CTRL_NOTE|PSP_CTRL_START,//LOOP
};

int GetControlValue(char *control_str)//should point to a button/pair ie NOTE|UP
{
char line[300];
char *str;
u32 pos;
int buttons = 0;
    pos = strcspn(control_str,"\n\r#");
    strncpy(line,control_str,pos);
    line[pos] = 0;
    str = strtok(line,"|");
    while(str != NULL)
    {
        if(stricmp(str,"SELECT")==0)
            buttons |= PSP_CTRL_SELECT;
        if(stricmp(str,"START")==0)
            buttons |= PSP_CTRL_START;
        if(stricmp(str,"UP")==0)
            buttons |= PSP_CTRL_UP;
        if(stricmp(str,"DOWN")==0)
            buttons |= PSP_CTRL_DOWN;
        if(stricmp(str,"LEFT")==0)
            buttons |= PSP_CTRL_LEFT;
        if(stricmp(str,"RIGHT")==0)
            buttons |= PSP_CTRL_RIGHT;
        if(stricmp(str,"LTRIGGER")==0)
            buttons |= PSP_CTRL_LTRIGGER;
        if(stricmp(str,"RTRIGGER")==0)
            buttons |= PSP_CTRL_RTRIGGER;
        if(stricmp(str,"TRIANGLE")==0)
            buttons |= PSP_CTRL_TRIANGLE;
        if(stricmp(str,"CIRCLE")==0)
            buttons |= PSP_CTRL_CIRCLE;
        if(stricmp(str,"CROSS")==0)
            buttons |= PSP_CTRL_CROSS;
        if(stricmp(str,"SQUARE")==0)
            buttons |= PSP_CTRL_SQUARE;
        if(stricmp(str,"HOME")==0)
            buttons |= PSP_CTRL_HOME;

        if(stricmp(str,"HOLD")==0) //not really a button
            buttons |= PSP_CTRL_HOLD;
        if(stricmp(str,"DISABLE")==0)//hold prevents any buttons from working so it acts as a disable switch
            return PSP_CTRL_HOLD;

        if(stricmp(str,"NOTE")==0)
            buttons |= PSP_CTRL_NOTE;

        if(stricmp(str,"SCREEN")==0)
            buttons |= PSP_CTRL_SCREEN;

        if(stricmp(str,"VOLUP")==0)
            buttons |= PSP_CTRL_VOLUP;

        if(stricmp(str,"VOLDOWN")==0)
            buttons |= PSP_CTRL_VOLDOWN;

        str = strtok(NULL,"| ");
    }

    return buttons;
}

char *GetConfigValue(char *config,char *option)
{
char line[300];
char *str;
u32 pos;

    str = strstr(config,option);
    if(str)
    {
        pos = strcspn(str,"\n\r#");
        strncpy(line,str,pos); //copy across the line
        line[pos] = '\0';
        pos = strlen(option);
        pos += strspn(&line[pos]," =\t");
        return str+pos;
    }
    else
        return NULL;

}

void LoadConfigFile(char *fname,MusicConf *config)
{
SceUID fd;
SceUID memid;
u32 filesize;
char *buf;
char *str;
int i,x;

	printf("Exec : LoadConfigFile()\n\n");
	
    memset(config,0,sizeof(MusicConf));
    fd = sceIoOpen(fname,PSP_O_RDONLY,0777);

    if(!(fd > -1))//default config
    {
        printf("Error : Could not open config file : %s\n\n", fname);
        config->found = 0;
        config->debug = 1;
        config->oc_wlan = 0;
        config->vsh_clock = 1;
        strcpy(config->dirname, MUSIC_DIR);
        for(i = 0; i < MAX_CONTROL_OPTIONS; i++)
            config->control[i] = DefaultControl[i];
        return;
    }
    //read file into buffer
    config->found = 1;
    filesize = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoLseek32(fd, 0, PSP_SEEK_SET);

    memid = sceKernelAllocPartitionMemory(1, "CONFIG_MEM", PSP_SMEM_Low, filesize, NULL);
    buf = sceKernelGetBlockHeadAddr(memid);

    sceIoRead(fd, buf, filesize);

    buf[filesize] = '\0'; //make the file a null-terminated string
    sceIoClose(fd);


    //first filter out comments
    for(i = 0; i < filesize; i++)
    {
        if(buf[i] == '#')
        {
            for(x = strcspn(&buf[i],"\n\r"); x > 0; x--)
                buf[i+x-1] = ' ';
        }
    }

    //parse control config
    for(i = 0; i < MAX_CONTROL_OPTIONS; i++)
    {
        str = GetConfigValue(buf, &ControlOptions[i][0]); 
        if(str)
            config->control[i] = GetControlValue(str);
        else
            config->control[i] = 0;
    }

    //parse other config values
    str = GetConfigValue(buf, "DEBUG"); 
    if(str)
        config->debug = atoi(str);
    else
        config->debug = 0;

        config->debug = 1;

    str = GetConfigValue(buf, "OC_WLAN"); 
    if(str)
        config->oc_wlan = atoi(str);
    else
        config->oc_wlan = 0;

    str = GetConfigValue(buf, "ENABLE_VSH_CLOCK"); 
    if(str)
        config->vsh_clock = atoi(str);
    else
        config->vsh_clock = 1;


    str = GetConfigValue(buf, "DEFAULT_MODE");
    if(!strncmp(str, "RAND", 4))
        music->random = 1;
    else
        music->random = 0;

    str = GetConfigValue(buf, "VOLUME"); 
    if(str)
    {
        music->volume = atoi(str);
        if ((music->volume < 0)||(music->volume > 100))
            music->volume = 100;
    }
    else
        music->volume = 100;

    str = GetConfigValue(buf, "CPU_SPEED"); 
    if(str)
    {
        cpu_speed[0] = atoi(str);
        set_cpu_speed = 1;
        
        if ((cpu_speed[0] < 1)||(cpu_speed[0] > 333))
            cpu_speed[0] = 222;
    }

    str = GetConfigValue(buf, "BUS_SPEED"); 
    if(str)
    {
        cpu_speed[1] = atoi(str);
        set_cpu_speed = 1;
        
        if ((cpu_speed[1] < 1)||(cpu_speed[1] > 167))
            cpu_speed[1] = 111;
    }

    str = GetConfigValue(buf, "DIR");
    if(str)
    {
        i = strcspn(str,"\n\r# ");
        str[i] = '\0';
        strcpy(config->dirname,str);
    }
    else
        strcpy(config->dirname, MUSIC_DIR);

    for(i = 0; i < MAX_CONTROL_OPTIONS; i++)//if control[i] is set to 0, make it equal to the default
    {
        if(config->control[i] == 0)
            config->control[i] = DefaultControl[i];
    }

    sceKernelFreePartitionMemory(memid);
	printf("Done : LoadConfigFile()\n\n");
}

