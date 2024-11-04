#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspaudio.h>
#include <pspmodulemgr.h>
#include <psploadcore.h>
#include <psppower.h>
#include <pspctrl.h>
#include <pspmscm.h>
#include <pspdisplay.h>
#include <pspumd.h>
#include <systemctrl.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "playback.h"
#include "main.h"
#include "config.h"
#include "blit.h"
#include "init.h"

#include "hw.h"
#include "hw_aa3.h"
#include "hw_at3.h"
#include "hw_mp3.h"

PSP_MODULE_INFO("Music_prx", 0x1000, 1, 1);

//for cpu speed control
u32 speed[SPEED_NUM] = {29,50,111,133,166,222,266,300,333};
u32 bus[SPEED_NUM] =   {14,25,55, 66, 83, 111,133,150,166};
u32 cpu_speed[2] = {222, 111};
char set_cpu_speed = 0;

int enable_blit;
int in_game_mute = 0;
SceUID main_thid;
MusicConf config;
//for on screen display
char str_buf[MAXPATH+1];
int blit_cpu_timer = 0;
int blit_mode_timer = 0;
int blit_ready_timer = DISPLAY_TIMER_AMT;
int blit_debug_timer = 0;
int hw_init = 0;

SceUID display_thid;

Playlist kmusic;
Playlist *music;


int module_start(SceSize args, void *argp)
{
    u8 st_world = 0;
    
	switch( sceKernelInitKeyConfig() ){
		case PSP_INIT_KEYCONFIG_VSH:  
			st_world = 1;
			break;
		case PSP_INIT_KEYCONFIG_POPS: 
			st_world = 2;
			break;
		case PSP_INIT_KEYCONFIG_GAME: 
			st_world = 3;
			break;
	}
	
    if( st_world == 1 ){
		while( sceKernelFindModuleByName( "sceVshBridge_Driver" ) == NULL ){
			sceKernelDelayThread( 2000000 );
		}
	} else if( st_world == 2 ){
		while( sceKernelFindModuleByName( "scePops_Manager" ) == NULL && sceKernelFindModuleByName( "popsloader_trademark" ) == NULL ){
			sceKernelDelayThread( 2000000 );
		}
	} else if(sceKernelBootFrom() == PSP_BOOT_DISC && sceKernelInitApitype() != PSP_INIT_APITYPE_DISC_UPDATER) {
		SceUID sfo;

		sceUmdActivate(1, "disc0:");
		sceUmdWaitDriveStat(PSP_UMD_READY);
		sceUmdDeactivate(1, "disc0:");
	}
    main_thid = sceKernelCreateThread("MAIN_Thread", main_thread, THREAD_PRIORITY+3, 0x4000, 0, NULL);
   	sceKernelStartThread(main_thid, args, argp);

    sceKernelExitDeleteThread(0);
    return 0;
}

int module_reboot_before(SceSize args, void *argp)
{
    sceKernelTerminateDeleteThread(main_thid);
    sceKernelTerminateDeleteThread(display_thid);
    sceKernelTerminateDeleteThread(music->mus_thid);

    TerminatePlaylist();

    sctrlHENSetSpeed(222, 111);

    return 0;
}

//returns number of music files in MUSIC_DIR and all subdirs 
int CountMusicFiles(char* dir)
{
	int fd;
    int music_cnt = 0;
    SceIoDirent d_dir;
    char tmp[MAXPATH];
    char ext[5];

    memset(&d_dir,0,sizeof(SceIoDirent));//prevents a crash
	fd = sceIoDopen(dir);

	if(fd >= 0)
	{
		while((sceIoDread(fd, &d_dir) > 0))
		{
			//if((d_dir.d_stat.st_attr & FIO_SO_IFDIR) == 0)//not a directory

            if(FIO_SO_ISDIR(d_dir.d_stat.st_attr) || FIO_S_ISDIR(d_dir.d_stat.st_mode))//handle sub dirs
            {
		if (d_dir.d_name[0] == '.') continue;

		printf("Directory spotted => %s\n\n", d_dir.d_name);
                sprintf(tmp, "%s%s/", dir, d_dir.d_name);
                music_cnt += CountMusicFiles(tmp);
            }

				else
			
			{
				if (d_dir.d_name[0] == '.') continue;

				printf("File Spoted => %s\n\n", d_dir.d_name);
               			memcpy(ext, d_dir.d_name + strlen(d_dir.d_name) - 4, 5);
                		if(!stricmp(ext, ".mp3") || !stricmp(ext, ".at3")
                 		||!stricmp(ext, ".aa3") || !stricmp(ext, ".oma")
                 		||!stricmp(ext, ".omg") ) //if it is an music file
                   		 music_cnt++;
				
			} 
		}

		sceIoDclose(fd);
	}
	printf("music_cnt = %i\n\n", music_cnt);
    return music_cnt;
}

//gets the name of an music file as a offset from the first music file in a directory and all subdirs
//filename is 262 (MAXPATH) bytes
int GetMusicFileName(char* dir,char* filename,int num,int basenum)
{
	int fd;
    int music_cnt;
    int music_dir_cnt;
    SceIoDirent d_dir;
    char tmp[MAXPATH];
    char ext[5];

    music_cnt = basenum;

    memset(&d_dir,0,sizeof(SceIoDirent));//prevents a crash
	fd = sceIoDopen(dir);

	if(fd >= 0)
	{
		while((sceIoDread(fd, &d_dir) > 0))
		{

		if(FIO_SO_ISDIR(d_dir.d_stat.st_attr) || FIO_S_ISDIR(d_dir.d_stat.st_mode))//handle sub dirs
            {
		if (d_dir.d_name[0] == '.') continue;
                sprintf(tmp, "%s%s/", dir, d_dir.d_name);
                music_dir_cnt = GetMusicFileName(tmp, filename, num, music_cnt);
                if(music_dir_cnt == -1) //found file
                    break;
                music_cnt = music_dir_cnt;
            }
		else
			//if((d_dir.d_stat.st_attr & FIO_SO_IFDIR) == 0)//not a directory
			{
		if (d_dir.d_name[0] == '.') continue;
                memcpy(ext, d_dir.d_name + strlen(d_dir.d_name) - 4, 5);
                if(!stricmp(ext, ".mp3") || !stricmp(ext, ".at3")
                 ||!stricmp(ext, ".aa3") || !stricmp(ext, ".oma")
                 ||!stricmp(ext, ".omg") ) //if it is an music file
                {
                    if(music_cnt == num)
                    {
                        sprintf(filename, "%s%s", dir, d_dir.d_name);
			printf("filename : %s\n\n", filename);
                        music_cnt = -1;
                        break;
                    }
                    music_cnt++;
                }
			} 
            //else if(d_dir.d_stat.st_attr & FIO_SO_IFDIR)//handle sub dirs

		}

		sceIoDclose(fd);
	}
    return music_cnt;
}

void InitPlaylist()
{
    music->omg_count = CountMusicFiles(OMG_AUDIO_DIR);
    music->count = CountMusicFiles(config.dirname) + music->omg_count;

    music->memid = sceKernelAllocPartitionMemory(1, "PLYLST_MEM", PSP_SMEM_Low, music->count*sizeof(int), NULL);
    music->random_played = sceKernelGetBlockHeadAddr(music->memid);

    music->offset = 0;
	music->resume = 0;

    music->pause = PLAYBACK_PAUSE_SONG;
	music->change_track = 0;

    music->loop = LOOP_NONE;

    ResetPlaylist();

    //create playlist thread
   	music->pl_thid = sceKernelCreateThread("Playlist_Thread", playlist_thread, THREAD_PRIORITY+2, 0x4000, 0, NULL);
   	if(music->pl_thid >= 0)
   		sceKernelStartThread(music->pl_thid, 0, NULL);
}

void TerminatePlaylist()
{
    sceKernelTerminateDeleteThread(music->pl_thid); 
    memset(music->file, 0, sizeof(music->file));
    sceKernelFreePartitionMemory(music->memid);
}

void ResetPlaylist()
{
int i;
    music->index = 0;
    music->random_index = 0;

    for(i = 0; i < music->count; i++)
        music->random_played[i] = -1; //played file not = to -1
}

int playlist_thread(SceSize args, void *argp)
{
	int i;
    char ext[5];
    char file_type;
    int ms_in;
    SceKernelThreadEntry music_thread = NULL;

    srand(time(NULL));

    while(1)
    {
        if (music->count)
        {
            if (music->random && (music->random_index >= music->count))
                ResetPlaylist(); 

            if ((!music->resume) && (music->loop != LOOP_SONG)) //not resuming music playback, load a new file
            {
                 //random file from dir
                if (music->random)
                {
                    music->index = rand()%music->count;

                    //check to see if this file was played before
                    for(i = music->random_index; i>=0; i--)
                    {
                        if (music->index == music->random_played[i])//if rand already played, pick a new one and recheck
                        {
                            music->index = rand()%music->count;
                            i = music->random_index;
                        }
                    }

                    music->random_played[music->random_index] = music->index;
                    music->random_index++;

                    if (music->index >= music->omg_count)
                        GetMusicFileName(config.dirname, music->file, music->index - music->omg_count, 0);
                    else
                        GetMusicFileName(OMG_AUDIO_DIR, music->file, music->index, 0);
                }
                else
                {
                    if (music->index >= music->omg_count)
                        GetMusicFileName(config.dirname, music->file, music->index - music->omg_count, 0);
                    else
                        GetMusicFileName(OMG_AUDIO_DIR, music->file, music->index, 0);

                    music->index++;

                    if (music->index >= music->count)
                       ResetPlaylist();
                }//if (music->random)
            }//if (!music->resume && music->loop == LOOP_NONE)

            music->title[0] = '\0'; //reset so the right title/filename is displayed

            if (music->loop == LOOP_IGNORE) //finished changing the track, reset the loop
                music->loop = LOOP_SONG;

        }//if (music->count)
        else
        {   /* if no files are in the playlist, sleep, 
            since the only way this can be fixed is by reloading */
            sceKernelSleepThreadCB();
        }

        memcpy(ext, music->file + strlen(music->file) - 4, 5);//get file extension

        //get the id3 title so the osd will have a real name to display 
        if (!stricmp(ext, ".aa3") || !stricmp(ext, ".oma") || !stricmp(ext, ".omg"))
            GetOMGTitle(music->file, music->title);

        //delay init'ing the hw decoder until play is pressed
        if (hw_init == 0) 
        {                     
            if (music->pause == PLAYBACK_PAUSE_SONG)
                music->pause = PLAYBACK_PAUSED;

            while( music->pause == PLAYBACK_PAUSED)
            {   //we must handle next/prev here too
                if ((music->change_track == PREV_SONG) || (music->change_track == NEXT_SONG)) 
                {
                    if (music->change_track == PREV_SONG)
                    { 
                        music->index -= 2;

                        if (music->index < 0)
                            music->index += music->count; //allows moving back in a loop
                    }

                    break;
                }
                sceKernelDelayThreadCB(DELAY_THREAD_AMT);
            }

            if ( music->change_track != 0)
            {
                music->change_track = 0;
                music->offset = 0;
                music->resume = 0;

                if (music->loop == LOOP_SONG)
                    music->loop = LOOP_IGNORE;

                continue;
            }
            hw_init = 1;
            HWInit();
        }
        
        //check to see if the music library is already loaded (keeps games from locking up)
        if ( FindProc("sceAvcodec_wrapper","sceAudiocodec",0x5B37EB1D)
          || FindProc("sceAudiocodec_Driver","sceAudiocodec",0x5B37EB1D) )
            music->init = 1;

        if (!stricmp(ext, ".mp3")) //if it is an mp3 file
            music_thread = HW_MP3PlayFile;
        else if (!stricmp(ext, ".at3")) //at3/at3+ file
            music_thread = HW_AT3PlayFile;
        else if (!stricmp(ext, ".aa3") || !stricmp(ext, ".oma") || !stricmp(ext, ".omg") )
        {
            file_type = GetOMGFileType(music->file);
            if (file_type == TYPE_AT3)
              music_thread = HW_AA3PlayFile;
            else if (file_type == TYPE_MP3)
                music_thread = HW_MP3PlayFile;
            else //invalid format
            {
                WaitMSReady();//wait until system is ready, this can happen due to suspend
                continue;
            }
        }

        music->flags = PLAYBACK_PLAYING;

        music->mus_thid = sceKernelCreateThread("music_thread", music_thread, THREAD_PRIORITY, 0x4000, 0, NULL);
        if (music->mus_thid >= 0)
            sceKernelStartThread(music->mus_thid, 0, NULL);

        //handle next, previous, pause resume
        while(1)//playing
        {
            if (music->pause == PLAYBACK_PAUSE_SONG)
            {
                sceKernelSuspendThread(music->mus_thid);
                music->pause = PLAYBACK_PAUSED;
            }
            else if (music->pause == PLAYBACK_RESUME_SONG)
            {
                sceKernelResumeThread(music->mus_thid);
                music->pause = PLAYBACK_PLAYING;
            }

            if ((music->change_track == PREV_SONG) || (music->change_track == NEXT_SONG)) 
            {
                if (music->change_track == PREV_SONG)
                { 
                    music->index -= 2;

                    if (music->index < 0)
                        music->index += music->count; //allows moving back in a loop
                }

                music->change_track = 0;
                music->offset = 0;
                music->resume = 0;

                if (music->loop == LOOP_SONG)
                    music->loop = LOOP_IGNORE;

                music->flags = PLAYBACK_DONE;//signal to the music thread that we are exiting

                if (music->pause == PLAYBACK_PAUSED)//resume thread if paused and then repause it 
                {
                    sceKernelResumeThread(music->mus_thid);
                    music->pause = PLAYBACK_PAUSE_SONG;
                }

                while(music->flags != PLAYBACK_CLEANED_UP)
                    sceKernelDelayThreadCB(DELAY_THREAD_AMT);//let it clean up

                sceKernelTerminateDeleteThread(music->mus_thid);
                break;
            }

            if (music->flags == PLAYBACK_CLEANED_UP)
            {
                sceKernelTerminateDeleteThread(music->mus_thid);
                do //check to see if the ms is ready (for suspend/resume)
                {
                    ms_in = MScmIsMediumInserted();
                    sceKernelDelayThreadCB(DELAY_THREAD_AMT);
                } while (ms_in <= 0);
                break;
            }

            sceKernelDelayThreadCB(DELAY_THREAD_AMT*5);
        } //while (1) //playing

    } //while (1) 

    return 0;
}

int main_thread(SceSize args, void *argp)
{
    SceCtrlData pad;
    u32 oldpad = 0;
    int speednum;
    int power_cbid;

    SceUID cpu_thid;

	SceModule *pMod;

    enable_blit = 0;

    //wait until memory stick is readable
    WaitMSReady();

    music = &kmusic;

    pMod = sceKernelFindModuleByName("Music_prx");//TODO Test in 1.5,
    music_text_addr = pMod->text_size;//pMod definition is wrong...
    music_text_end = pMod->text_size+pMod->data_size;

    LoadConfigFile("ms0:/seplugins/music_conf.txt", &config);

    power_cbid = sceKernelCreateCallback("powercb", (SceKernelCallbackFunction)PowerCallback, NULL);
    scePowerRegisterCallback(15, power_cbid);

    music->is_vsh = 0;
    music->mus_thid = -1;
    music->init = 0;

	printf("Exec : InitPatches()\n\n");
    InitPatches();
	printf("Done : InitPatches()\n\n");
	printf("Exec : InitPlaylist()\n\n");
    InitPlaylist();
	printf("Done : InitPlaylist()\n\n");

    display_thid = sceKernelCreateThread("DisplayThread", display_thread, THREAD_PRIORITY+2, 0x4000, 0, NULL);//higher priority reduces flickering
    if (display_thid >= 0)
        sceKernelStartThread(display_thid, 0, NULL);

    if (set_cpu_speed)
    {
        cpu_thid = sceKernelCreateThread("CPUThread", cpu_thread, THREAD_PRIORITY+3, 0x2000, 0, NULL);
        if (cpu_thid >= 0)
            sceKernelStartThread(cpu_thid, 0, NULL); 
    }

    while(1)
    {
        sceKernelDelayThreadCB(5*DELAY_THREAD_AMT);//needs less running time

        sceCtrlPeekBufferPositive(&pad, 1); 
        if(pad.Buttons != 0)
        {
            if((pad.Buttons & config.control[OPT_VOLUP]) == config.control[OPT_VOLUP])//allow repeated presses for volume
            {
                if(music->volume<100)//max is 100
                    music->volume++;
            }
            if((pad.Buttons & config.control[OPT_VOLDOWN]) == config.control[OPT_VOLDOWN])
            {
                if(music->volume>0)//min is 0
                    music->volume--;
            }   
        
            if(pad.Buttons != oldpad)
            {
                oldpad = pad.Buttons;

                if((pad.Buttons & config.control[OPT_START_STOP]) == config.control[OPT_START_STOP])
                {
                    if (music->pause == PLAYBACK_PAUSED)
                        music->pause = PLAYBACK_RESUME_SONG;
                    else if (music->pause == PLAYBACK_PLAYING)
                        music->pause = PLAYBACK_PAUSE_SONG;
                }

                if ((pad.Buttons & config.control[OPT_PREVIOUS]) == config.control[OPT_PREVIOUS])
                {
                    if(!music->random)
                        music->change_track = PREV_SONG;
                }

                if ((pad.Buttons & config.control[OPT_NEXT]) == config.control[OPT_NEXT])
                    music->change_track = NEXT_SONG;

                if ((pad.Buttons & config.control[OPT_MODE_TOGGLE]) == config.control[OPT_MODE_TOGGLE])
                {
                    music->random = !music->random;
                    ResetPlaylist();
                    music->change_track = NEXT_SONG;

                    blit_mode_timer = DISPLAY_TIMER_AMT;
                }

                if ((pad.Buttons & config.control[OPT_LOOP]) == config.control[OPT_LOOP])
                {
                    if (music->loop == LOOP_NONE)
                        music->loop = LOOP_SONG;
                    else if((music->loop == LOOP_SONG) || (music->loop == LOOP_IGNORE))
                        music->loop = LOOP_NONE;
                }

                if ((pad.Buttons & config.control[OPT_CPU_NEXT]) == config.control[OPT_CPU_NEXT])
                {
                    speednum = GetNextSpeedNum(SPEED_NEXT);

                    if (speednum >= 5)//for speeds >=222
                        sctrlHENSetSpeed(speed[speednum], bus[speednum]);
                    else if (speednum != -1)//for speeds <222
                    {
                        sctrlHENSetSpeed(222, 111);//fix allows cpu speeds to be set properly
                        sceKernelDelayThreadCB(DELAY_THREAD_AMT);
                        sctrlHENSetSpeed(speed[speednum], bus[speednum]);
                    }

                    blit_cpu_timer = DISPLAY_TIMER_AMT;
                }

                if ((pad.Buttons & config.control[OPT_CPU_PREV]) == config.control[OPT_CPU_PREV])
                {
                    speednum = GetNextSpeedNum(SPEED_PREV);

                    if (speednum >= 5)//for speeds >=222
                        sctrlHENSetSpeed(speed[speednum], bus[speednum]);
                    else if (speednum != -1)//for speeds <222
                    {
                        sctrlHENSetSpeed(222, 111);//fix allows cpu speeds to be set properly
                        sceKernelDelayThreadCB(DELAY_THREAD_AMT);
                        sctrlHENSetSpeed(speed[speednum], bus[speednum]);
                    }
                 
                    blit_cpu_timer = DISPLAY_TIMER_AMT;
                }

                if ((pad.Buttons & config.control[OPT_RELOAD]) == config.control[OPT_RELOAD])
                {
                    //reload the playlist and its handler
                    TerminatePlaylist();

                    //resume music_thread so it will run the cleanup code (if it exists)
                    if(sceKernelResumeThread(music->mus_thid) != SCE_KERNEL_ERROR_UNKNOWN_THID)
                    {
                        //unpause so the thread can clean up
                        music->flags = PLAYBACK_DONE;

                        while(music->flags != PLAYBACK_CLEANED_UP)
                            sceKernelDelayThreadCB(DELAY_THREAD_AMT);

                        sceKernelTerminateDeleteThread(music->mus_thid);
                    }

                    LoadConfigFile("ms0:/seplugins/music_conf.txt", &config);

                    InitPlaylist();

                    blit_ready_timer = DISPLAY_TIMER_AMT;
                }

                if ((pad.Buttons & config.control[OPT_DISPLAY]) == config.control[OPT_DISPLAY])
                {
                    enable_blit ^= 1;
                    if(config.debug)
                        blit_debug_timer ^= 1;
                }
                if ((pad.Buttons & config.control[OPT_IN_GAME_MUTE]) == config.control[OPT_IN_GAME_MUTE])
                {
                    if (in_game_mute)
                    {
                        in_game_mute = 0;
//                        if (music->output2init)//TODO FIX THIS
//                        AudioOutput2RestoreSettings();
                    }
                    else 
                        in_game_mute = 1;
                }
            }        
        }
    }
}

void WaitMSReady()//wait until ms is ready
{
	printf("\nExec : WaitMSReady()\n\n");
	int ret;
    ret = MScmIsMediumInserted();
    while(ret <= 0)
    {
        sceKernelDelayThreadCB(DELAY_THREAD_AMT);
        ret = MScmIsMediumInserted();
    }
    sceKernelDelayThreadCB(DELAY_THREAD_SEC);
	printf("Done : WaitMSReady()\n\n");
	
}

int display_thread(SceSize args, void *argp)
{
    int i;
    SceKernelThreadInfo th_info;
    char *fname;
    while(1)
    {

        if (config.debug && blit_debug_timer)
        {
            sprintf(str_buf, "pl_cur:%08X flag:%08X init:%08X mid:%08X ", music->index, music->flags, music->init, music->mus_thid);
            blit_string(0, 18, str_buf, 0xffffff, 0x000000);

            sprintf(str_buf, "list:%08X off:%08X", music->count, music->offset);
            blit_string(50-18, 31, str_buf, 0xffffff, 0x000000);

            memset(&th_info, 0, sizeof(th_info));
            th_info.size = sizeof(th_info);
            sceKernelReferThreadStatus(music->mus_thid, &th_info);

            sprintf(str_buf, "Music Status:%08X", th_info.status);
            blit_string(0, 20, str_buf, 0xffffff, 0x000000);

            memset(&th_info, 0, sizeof(th_info));
            th_info.size = sizeof(th_info);
            sceKernelReferThreadStatus(music->pl_thid, &th_info);

            sprintf(str_buf, "Plylst Status:%08X", th_info.status);
            blit_string(0, 21, str_buf, 0xffffff, 0x000000);

            for(i=0;i<9;i++)
            {     
                sprintf(str_buf, "mp3:%d:%08X", i, music->deb[i]/* sceAudioChangeChannelConfig(i, 0)*/);
                blit_string(50-18, 20+i, str_buf, 0xffffff, 0x000000);
            }

            sprintf(str_buf, "mp3:%08X", music->audio_id);
            blit_string(50-18, 29, str_buf, 0xffffff, 0x000000);
        }
        if(enable_blit || blit_mode_timer)
        {
            if(music->random)
                blit_string(8, 32, "RAND", 0xffffff, 0x000000);
            else
                blit_string(8, 32, "SEQ", 0xffffff, 0x000000);

            if(blit_mode_timer)
                blit_mode_timer--;
        }

        if(enable_blit || blit_cpu_timer)
        {
            sprintf(str_buf, "%03d:%03d", scePowerGetCpuClockFrequency(), scePowerGetBusClockFrequency());
            blit_string(18, 32, str_buf, 0xffffff, 0x000000);

            if(blit_cpu_timer)
                blit_cpu_timer--;
        }

        if(enable_blit)
        {
            fname = music->file;

            if (strstr(fname,config.dirname))
                blit_string(0, 33, &music->file[strlen(config.dirname)], 0xffffff, 0x000000);
            else if(music->title[0] != 0) //from omgaudio dir, use ID3 TIT2 
                blit_string(0, 33, music->title, 0xffffff, 0x000000);
            else
                blit_string(0, 33, &music->file[14], 0xffffff, 0x000000); //14 = strlen(OMG_AUDIO_DIR)

            if ((music->loop == LOOP_SONG) || (music->loop == LOOP_IGNORE))
                blit_string(13, 32, "LOOP", 0xffffff, 0x000000);

            sprintf(str_buf, "vol: %d", music->volume);
            blit_string(26, 32, str_buf, 0xffffff, 0x000000);

            if (music->pause == PLAYBACK_PAUSED || music->pause == PLAYBACK_PAUSE_SONG)
                blit_string(0, 32, "PAUSED", 0xffffff, 0x000000);
            else if (music->pause == PLAYBACK_PLAYING || music->pause == PLAYBACK_RESUME_SONG)
                blit_string(0, 32, "PLAYING", 0xffffff, 0x000000);

            if (in_game_mute)
                blit_string(35, 32, "IGM", 0xffffff, 0x000000);
        }

        sceKernelDelayThreadCB(DELAY_THREAD_AMT/2);
    }
}

SceUID LoadStartModule(char *modname, int partition)
{
    SceKernelLMOption option;
    SceUID modid;

    memset(&option, 0, sizeof(option));
    option.size = sizeof(option);
    option.mpidtext = partition;
    option.mpiddata = partition;
    option.position = 0;
    option.access = 1;

    modid = sceKernelLoadModule(modname, 0, &option);
    if (modid < 0)
        return modid;

    return sceKernelStartModule(modid, 0, NULL, NULL, NULL);
}

void PowerCallback(int unknown, int powerInfo)
{
    if ( (powerInfo & PSP_POWER_CB_POWER_SWITCH)
       ||(powerInfo & PSP_POWER_CB_SUSPENDING)
       ||(powerInfo & PSP_POWER_CB_STANDBY) )
    {
        music->flags = PLAYBACK_RESET;
    }
}

int cpu_thread(SceSize args, void *argp)
{
    char *vram32;
    while(1)
    {
        sceKernelDelayThreadCB(5*DELAY_THREAD_AMT);//needs less running time

        sceDisplayGetFrameBuf((void*)&vram32, NULL, NULL, NULL);
        if (vram32)//the display and therefore the system is ready
        {
            sceKernelDelayThreadCB(DELAY_THREAD_SEC*2);

            sctrlHENSetSpeed(222, 111);//fix allows cpu speeds to be set properly
            sceKernelDelayThreadCB(DELAY_THREAD_AMT);

            sctrlHENSetSpeed(cpu_speed[0], cpu_speed[1]);

            set_cpu_speed = 0;
            sceKernelExitDeleteThread(0);
        }
    }
}

u32 GetNextSpeedNum(int dir)
{
    int i;
    u32 current_speed;
    u32 speed_diff[SPEED_NUM]; 

    current_speed = scePowerGetCpuClockFrequency();

    if (dir == SPEED_NEXT) //search for the closest cpu speed, then return the next/prev one
    {
        for (i = 0; i < SPEED_NUM; i++)
            speed_diff[i] = abs(current_speed - speed[i]);
        for (i = 1; i < (SPEED_NUM+1); i++)
        {
            if (speed_diff[i%SPEED_NUM] > speed_diff[(i-1)%SPEED_NUM])
                return i%SPEED_NUM;
        }
    }
    else if (dir == SPEED_PREV)
    {
        for (i = 0; i < SPEED_NUM; i++)
           speed_diff[i] = abs(current_speed - speed[i]);
        for (i = (SPEED_NUM-1); i > 0; i--)
        {
            if (speed_diff[(i-1)%SPEED_NUM] > speed_diff[i%SPEED_NUM])
                return i-1;
        }
        if (speed_diff[SPEED_NUM-1] > speed_diff[0])
            return SPEED_NUM-1;
    }
    return -1;
}

int GetID3TagSize(char *fname)
{
    SceUID fd;
    char header[10];
    int size = 0;
    fd = sceIoOpen(fname, PSP_O_RDONLY, 0777);
    if (fd < 0)
        return 0;

    sceIoRead(fd, header, sizeof(header));
    sceIoClose(fd);

    if (!strncmp((char*)header, "ea3", 3) || !strncmp((char*)header, "EA3", 3)
      ||!strncmp((char*)header, "ID3", 3))
    {
        //get the real size from the syncsafe int
        size = header[6];
        size = (size<<7) | header[7];
        size = (size<<7) | header[8];
        size = (size<<7) | header[9];

        size += 10;

        if (header[5] & 0x10) //has footer
            size += 10;
         return size;
    }
    return 0;
}

// uses ID3(ea3) TIT2 to get the song's name (this is only for sonicstage copied
// audio, so we don't display 1XXXXXXX.OMA as the name)
void GetOMGTitle(char *fname, char *title)
{
    SceUID memid;
    unsigned char *pBuffer;
    int i,x;
    int current_char = 0;
    int size = 0;
    int maxsize = 0;
    unsigned char character;
    SceUID fd;

    maxsize = GetID3TagSize(fname);

    fd = sceIoOpen(fname, PSP_O_RDONLY, 0777);
    if (fd < 0)
        return;

    memid = sceKernelAllocPartitionMemory(1, "id3", PSP_SMEM_Low, maxsize, NULL);
    pBuffer = (u8*)sceKernelGetBlockHeadAddr(memid);

    size = sceIoRead(fd, pBuffer, maxsize);
    sceIoClose(fd);
    if (size <= 0)
    {
        sceKernelFreePartitionMemory(memid);
        return;
    }
    
    for (i = 0; i < size; i++)
    {
        //if this is a TIT2 frame
        if ( !strncmp((char*)pBuffer+i, "TIT2", 4))
        {
            size = pBuffer[i+4]; //size of title
            size = (size<<7) | pBuffer[i+5];
            size = (size<<7) | pBuffer[i+6];
            size = (size<<7) | pBuffer[i+7];

            if (size > MAXPATH)
                size = MAXPATH;

            memset(title, 0, MAXPATH);

            for (x = 1; x < size; x++) //this allows us to parse latin-encoded 
            {                          //unicode strings, though it could be better
                character = pBuffer[i+x+10]; //buf offset + str offset + frame header size
                if ((character >= 0x20) && (character <= 0x7f))
                {  
                    title[current_char] = character;
                    current_char++;
                }
            }
            break;
        }
    }

    sceKernelFreePartitionMemory(memid);
}

char GetOMGFileType(char *fname)
{
    SceUID fd;
    int size;
    char ea3_header[0x60];

    size = GetID3TagSize(fname);

    fd = sceIoOpen(fname, PSP_O_RDONLY, 0777);
    if (fd < 0)
        return TYPE_UNK;

    sceIoLseek32(fd, size, PSP_SEEK_SET);

    if (sceIoRead(fd, ea3_header, 0x60) != 0x60)
        return TYPE_UNK;

    sceIoClose(fd);

    if (strncmp(ea3_header, "EA3", 3) != 0)
        return TYPE_UNK;

    switch (ea3_header[3])
    {
        case 1:
        case 3:
            return TYPE_AT3;
        case 2:
            return TYPE_MP3;
        default:
            return TYPE_UNK;
    }
}

