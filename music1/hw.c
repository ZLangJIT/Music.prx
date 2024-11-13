#include <pspsdk.h>
#include <pspaudio.h>
#include <pspsysmem.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <psputility_avmodules.h>
#include <pspaudiocodec.h>

#include <string.h>

#include "main.h"

#include "playback.h"
#include "hw.h"
#include "log.h"

//shared global vars
SceUID fd;
u16 data_align;
u32 sample_per_frame;
u16 channel_mode;
u32 samplerate;
long data_start;
long data_size;
u8 getEDRAM;

SceUID data_memid;
volatile int OutputBuffer_flip=0;
//shared between at3+aa3
u16 at3_type;
u8* at3_data_buffer;
u8 at3_at3plus_flagdata[2];
unsigned char   AT3_OutputBuffer[2][AT3_OUTPUT_BUFFER_SIZE]__attribute__((aligned(64))),
                *AT3_OutputPtr=AT3_OutputBuffer[0];

CodecData *codec;

int HWInit()
{
    data_memid = sceKernelAllocPartitionMemory(2, "mem", PSP_SMEM_Low, sizeof(CodecData), NULL);
    codec = (CodecData *)sceKernelGetBlockHeadAddr(data_memid);
    codec = (CodecData *)(((u32)codec+64) & 0xFFFFFFC0);

    return 0;
}

void HW_LoadModules()
{
	int ret;
        ret = sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
	if(ret==0) printf("Success : sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);\n\n");
        music->init = 1;
}

void pspOpenAudio(int samplecount)
{
    if(music->is_vsh)
    {
        sceAudioChRelease(4);//browser bug fix, it seems to allocate all channels, free an unneeded one for playback in vsh mode
        music->audio_id = sceAudioChReserve(4, samplecount, PSP_AUDIO_FORMAT_STEREO );
	printf("sceAudioChReserve(4, samplecount, PSP_AUDIO_FORMAT_STEREO );\n\n");
    }
    else
	{
		//sceAudioChRelease(7);
        	music->audio_id = sceAudioChReserve(7, PSP_AUDIO_SAMPLE_ALIGN(samplecount), PSP_AUDIO_FORMAT_STEREO );
		printf("sceAudioChReserve(7, samplecount, PSP_AUDIO_FORMAT_STEREO );\n\n");
	}

    if(music->audio_id < 0)   
	{
        	music->audio_id = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, samplecount , PSP_AUDIO_FORMAT_STEREO );
		printf("sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, samplecount, PSP_AUDIO_FORMAT_STEREO );\n\n");
	}
    if(music->audio_id < 0)   
	{
		printf("Could not allocate audio channel\n\n");
	}
}

void pspCloseAudio()
{
	sceAudioChRelease(music->audio_id);
    music->audio_id = 9; //invalid channel so in_game_mute doesn't allow an old channel to output sound
}
