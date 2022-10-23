	.set noreorder

#include "pspimport.s"

	IMPORT_START	"sceCtrl_driver",0x00010000
	IMPORT_FUNC	"sceCtrl_driver",0x2BA616AF,sceCtrlPeekBufferPositive
