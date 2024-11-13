	.set noreorder

#include "pspimport.s"

	IMPORT_START	"sceAudio_driver",0x00010000
    IMPORT_FUNC "sceAudio_driver",0x9D77949E,sceAudio_driver_9D77949E
    IMPORT_FUNC "sceAudio_driver",0x837701CC,sceAudio_driver_837701CC
    IMPORT_FUNC "sceAudio_driver",0xB7F5A1B2,sceAudio_driver_B7F5A1B2
    IMPORT_FUNC "sceAudio_driver",0xAC81DE4F,sceAudio_driver_AC81DE4F
