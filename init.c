#include <pspsdk.h>
#include <psppower.h>
#include <pspsysmem.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <string.h>
#include <pspaudio.h>
#include <pspkerror.h>
#include "playback.h"

#include "init.h"
#include "config.h"
#include "hw.h"
#include "blit.h"

APRS_EVENT previous_func;

extern MusicConf config;
extern int in_game_mute;

u32 music_text_addr;
u32 music_text_end;

int old_samplecount = 0;
int old_samplerate = 0;
int old_unk = 0;

u32 (*DcacheWritebackInvalidateAll)();//for some reason this messes up the 
                                      //library imports if it is called normally

int AudioHook_func(int channel)//we only need a0
{
   if(in_game_mute == 0)//allow all output if igm is off
       return 1;
   if(channel == music->audio_id)
       return 1;
   return 0; //block any output by other channels when igm is on
}

char IsHWPrxCall(void *ra)
{
    if (((u32)ra >= music_text_addr) && ((u32)ra <= music_text_end))
        return 1;
    
    return 0;
}

void AudioHook2Reserve_func(int samplecount, int samplerate, int unk, void *ra)
{
    int ret;
    if (!IsHWPrxCall(ra))
    {
        old_samplecount = samplecount;
        old_samplerate = samplerate;
        old_unk = unk;
    }

    do
    {
        ret = sceAudio_driver_B7F5A1B2();//sceAudioOutput2Release
        sceKernelDelayThreadCB(DELAY_THREAD_AMT*2);
    } while ((ret < 0) && (ret != 0x80260008));//0x80260008 = error not reserved

}

int AudioHook2Output_func(int vol, char *samples, void *ra)
{
    if (in_game_mute)
    {
        if (IsHWPrxCall(ra))
            return 1;
        else
            return 0;
    }

    return 1;
}

void AudioOutput2RestoreSettings()
{
    if(old_samplecount)
        sceAudio_driver_837701CC(old_samplecount, old_samplerate, old_unk);
}

int AudioNids[AUDIO_FUNC_NUM] = {
0x671E97E8,//sceAudioOutput
0x5CDEF9A4,//sceAudioOutputBlocking
0xE440DF7D,//sceAudioOneshotOutput
0x17D856B9,//sceAudioOutputPanned
0xB7CCF1D7 //sceAudioOutputPannedBlocking
};

u32 AudioHooks[AUDIO_FUNC_NUM][30];
u32 AudioHook2[2][19];

void InitPatches()
{
    u32 *MaxBrightnessFunc;
    u32 *AudioFuncs[AUDIO_FUNC_NUM];
    u32 *AudioFunc2;
    int i;
    APRS_EVENT (*sctrlHENSetOnApplyPspRelSectionEvent)(APRS_EVENT);

    //returns 3 if psp isn't plugged in, 4 if it is
    //patch to allow 4th brightness level (make function return 4)
    MaxBrightnessFunc = (u32*)FindProc("scePower_Service","scePower_driver",0x442BFBAC);
    if(MaxBrightnessFunc != 0)
    {
        MaxBrightnessFunc[0] = MIPS_LUI(2,0);
        MaxBrightnessFunc[1] = MIPS_ORI(2,2,4); //li $v0, 4
        MaxBrightnessFunc[2] = MIPS_JR(31);     //jr $ra
        MaxBrightnessFunc[3] = MIPS_NOP;
    }

    //it's done this way to keep 1.5 mode compatibility
    sctrlHENSetOnApplyPspRelSectionEvent = (APRS_EVENT (*)(APRS_EVENT)) FindProc("SystemControl","SystemCtrlForKernel",0x4EB249E4);
    if(sctrlHENSetOnApplyPspRelSectionEvent != 0)
        previous_func = (*sctrlHENSetOnApplyPspRelSectionEvent)(OnPspRelSectionEvent);

    for(i = 0; i < AUDIO_FUNC_NUM; i++)
    {

        AudioFuncs[i] = (u32*)FindProc("sceAudio_Driver","sceAudio_driver",AudioNids[i]);

        if(AudioFuncs[i] != 0)
        {

            AudioHooks[i][0] = MIPS_ADDIU(29,29,-28); //to preserve args
            AudioHooks[i][1] = MIPS_SW(4,29,0);
            AudioHooks[i][2] = MIPS_SW(5,29,4);
            AudioHooks[i][3] = MIPS_SW(6,29,8);
            AudioHooks[i][4] = MIPS_SW(7,29,12);
            AudioHooks[i][5] = MIPS_SW(8,29,16);
            AudioHooks[i][6] = MIPS_SW(9,29,20);
            AudioHooks[i][7] = MIPS_SW(31,29,24);

            AudioHooks[i][8] = MIPS_JAL(&AudioHook_func);
            AudioHooks[i][9] = MIPS_NOP;

            AudioHooks[i][10] = MIPS_LW(4,29,0);
            AudioHooks[i][11] = MIPS_LW(5,29,4);
            AudioHooks[i][12] = MIPS_LW(6,29,8);
            AudioHooks[i][13] = MIPS_LW(7,29,12);
            AudioHooks[i][14] = MIPS_LW(8,29,16);
            AudioHooks[i][15] = MIPS_LW(9,29,20);
            AudioHooks[i][16] = MIPS_LW(31,29,24);
            AudioHooks[i][17] = MIPS_ADDIU(29,29,28);

            AudioHooks[i][18] = 0x10400005;//MIPS_BEQ(2,0,6*4);
            AudioHooks[i][19] = MIPS_NOP;

            AudioHooks[i][20] = AudioFuncs[i][0]; //copy orig ins
            AudioHooks[i][21] = AudioFuncs[i][1];
            AudioHooks[i][22] = MIPS_J(&AudioFuncs[i][2]);
            AudioHooks[i][23] = MIPS_NOP;
            //branch target
            switch (AudioNids[i])
            {
                case 0x671E97E8: //sceAudioOutput
                case 0x5CDEF9A4: //sceAudioOutputBlocking
                {
                    AudioHooks[i][24] = MIPS_ADDIU(5,0,0);//set volume to 0
                    AudioHooks[i][25] = MIPS_NOP;                
                } break;
                case 0xE440DF7D: //sceAudioOneshotOutput (not tested, but should work)
                {
                    AudioHooks[i][24] = MIPS_ADDIU(7,0,0);//set left+right volume to 0
                    AudioHooks[i][25] = MIPS_ADDIU(8,0,0);                
                } break;
                case 0x17D856B9: //sceAudioOutputPanned
                case 0xB7CCF1D7: //sceAudioOutputPannedBlocking
                {   
                    AudioHooks[i][24] = MIPS_ADDIU(5,0,0);//set left+right volume to 0
                    AudioHooks[i][25] = MIPS_ADDIU(6,0,0);           
                } break;
            }

            AudioHooks[i][26] = AudioFuncs[i][0]; //copy orig ins
            AudioHooks[i][27] = AudioFuncs[i][1];
            AudioHooks[i][28] = MIPS_J(&AudioFuncs[i][2]);
            AudioHooks[i][29] = MIPS_NOP;

            //replace orig ins
            AudioFuncs[i][0] = MIPS_J(&AudioHooks[i][0]);
            AudioFuncs[i][1] = MIPS_NOP;
        }
    }

    //allows !=44.1kHz music to play without messing up the vsh's video/music player
    //as long as the song is paused when using the vsh's version
    AudioFunc2 = (u32*)FindProc("sceAudio_Driver","sceAudio_driver",0x837701CC);//called by sceAudioOutput2Reserve
    if(AudioFunc2 != 0)
    {
        AudioHook2[0][0] = MIPS_ADDIU(7,31,0);
        AudioHook2[0][1] = MIPS_ADDIU(29,29,-16); //to preserve args
        AudioHook2[0][2] = MIPS_SW(4,29,0);
        AudioHook2[0][3] = MIPS_SW(5,29,4);
        AudioHook2[0][4] = MIPS_SW(6,29,8);
        AudioHook2[0][5] = MIPS_SW(31,29,12);

        AudioHook2[0][6] = MIPS_JAL(&AudioHook2Reserve_func);
        AudioHook2[0][7] = MIPS_NOP;

        AudioHook2[0][8] = MIPS_LW(4,29,0);
        AudioHook2[0][9] = MIPS_LW(5,29,4);
        AudioHook2[0][10] = MIPS_LW(6,29,8);
        AudioHook2[0][11] = MIPS_LW(31,29,12);
        AudioHook2[0][12] = MIPS_ADDIU(29,29,16);

        AudioHook2[0][13] = AudioFunc2[0]; //copy orig ins
        AudioHook2[0][14] = AudioFunc2[1];
        AudioHook2[0][15] = MIPS_J(&AudioFunc2[2]);
        AudioHook2[0][16] = MIPS_NOP;

        //replace orig ins
        AudioFunc2[0] = MIPS_J(&AudioHook2[0][0]);
        AudioFunc2[1] = MIPS_NOP;
    }

    AudioFunc2 = (u32*)FindProc("sceAudio_Driver", "sceAudio_driver", 0xAC81DE4F);//called by sceAudioOutput2OutputBlocking
    if(AudioFunc2 != 0)
    {
        AudioHook2[1][0] = MIPS_ADDIU(6,31,0);
        AudioHook2[1][1] = MIPS_ADDIU(29,29,-12); //to preserve args
        AudioHook2[1][2] = MIPS_SW(4,29,0);
        AudioHook2[1][3] = MIPS_SW(5,29,4);
        AudioHook2[1][4] = MIPS_SW(31,29,8);

        AudioHook2[1][5] = MIPS_JAL(&AudioHook2Output_func);
        AudioHook2[1][6] = MIPS_NOP;

        AudioHook2[1][7] = MIPS_LW(4,29,0);
        AudioHook2[1][8] = MIPS_LW(5,29,4);
        AudioHook2[1][9] = MIPS_LW(31,29,8);
        AudioHook2[1][10] = MIPS_ADDIU(29,29,12);

        AudioHook2[1][11] = 10400006; //10400005;//MIPS_BEQ(2,0,6*4);
        AudioHook2[1][12] = MIPS_NOP;

        AudioHook2[1][13] = AudioFunc2[0]; //copy orig ins
        AudioHook2[1][14] = AudioFunc2[1];
        AudioHook2[1][15] = MIPS_J(&AudioFunc2[2]);
        AudioHook2[1][16] = MIPS_NOP;
        //branch target
        AudioHook2[1][17] = MIPS_JR(31); //return 0, block output
        AudioHook2[1][18] = MIPS_ADDIU(2,0,0);

        //replace orig ins
        AudioFunc2[0] = MIPS_J(&AudioHook2[1][0]);
        AudioFunc2[1] = MIPS_NOP;
    }

    DcacheWritebackInvalidateAll = (u32 (*)()) FindProc("sceKernelUtils", "UtilsForKernel", 0xB435DEC5);
    if(DcacheWritebackInvalidateAll)
        (*DcacheWritebackInvalidateAll)();

}

/* New FindProc based on tyranid's psplink code. PspPet one doesn't work
   well with 2.7X+ sysmem.prx */
u32 FindProc(const char* szMod, const char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;
	pMod = sceKernelFindModuleByName(szMod);

	if (!pMod)
        return 0;
	
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;

	while(i < entLen)
	{
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);
        	if(entry->libname && !strcmp(entry->libname, szLib))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;
			if(entry->stubcount > 0)
			{
				for(count = 0; count < entry->stubcount; count++)
				{
					if (vars[count] == nid)
						return vars[count+total];
				}
			}
		}

		i += (entry->len * 4);
	}

	return 0;
}

//for various patches loaded upon start-up
int OnPspRelSectionEvent(char *modname, u8 *modbuf)
{
    u32 *ClockFunc;
    int i;
    SceUID ClockNids[3] = {0xA4E93389, 0xEBD177D6, 0x545A7F3C};

    if (strcmp(modname, "vsh_module") == 0)
    {
        if (config.vsh_clock == 0)
        {
            for (i = 0; i < 3; i++)
            {
                ClockFunc = (u32*)FindProc("scePower_Service", "scePower_driver", ClockNids[i]);
                if (ClockFunc != 0)//this disables vsh from messing with the clock speed
                {
                    ClockFunc[0] = MIPS_NOP;   //return 0
                    ClockFunc[1] = MIPS_JR(31);
                    ClockFunc[2] = MIPS_ADDIU(2, 0, 0);
                }
            }
            //allows the browser to be "overclocked"
            ClockFunc = (u32*)FindProc("scePower_Service", "scePower_driver", 0xE52B4362);
            if (ClockFunc != 0)
            {
                ClockFunc[0] = MIPS_NOP;   //return 0
                ClockFunc[1] = MIPS_JR(31);
                ClockFunc[2] = MIPS_ADDIU(2, 0, 0);
            }
            if(DcacheWritebackInvalidateAll)
                    (*DcacheWritebackInvalidateAll)();  
        }
        music->is_vsh = 1;
    }

    if (!previous_func)
        return 0;
   
    return previous_func(modname, modbuf);
}
