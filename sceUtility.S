	.set noreorder

#include "pspimport.s"

	IMPORT_START	"sceUtility",0x00090000
	IMPORT_FUNC	"sceUtility",0xC629AF26,sceUtilityLoadAvModule
	IMPORT_FUNC	"sceUtility",0xF7D8D092,sceUtilityUnloadAvModule
