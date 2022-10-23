	.set noreorder

#include "pspimport.s"

	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xF6B1BF0F,sceKernelFindModuleByName
