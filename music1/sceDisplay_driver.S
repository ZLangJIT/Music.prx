	.set noreorder

#include "pspimport.s"

	IMPORT_START	"sceDisplay_driver",0x00010000
	IMPORT_FUNC	"sceDisplay_driver",0xDEA197D4,sceDisplayGetMode
	IMPORT_FUNC	"sceDisplay_driver",0xFBB369FD,sceDisplayGetFrameBuf
