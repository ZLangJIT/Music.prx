	.set noreorder

#include "pspimport.s"

	IMPORT_START	"scePower_driver",0x00010000
	IMPORT_FUNC	"scePower_driver",0x766CD857,scePowerRegisterCallback
	IMPORT_FUNC	"scePower_driver",0xFDB5BFE9,scePowerGetCpuClockFrequency
 	IMPORT_FUNC	"scePower_driver",0x04711DFB,scePowerGetBusClockFrequency
