#include <pspkerneltypes.h>

#include <string.h>
#include <stdlib.h>

void _log(char *fmt, ...) {
	va_list list;
	char data[4096];

	va_start(list, fmt);
	vsnprintf(data, 4096, fmt, list);
	va_end(list);

	_log_file(data);
	//_log_psplink(data);
}

void _log_file(char *data) {
	SceUID fd = sceIoOpen("ms0:/music.prx.log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
	if(fd > -1) {
		sceIoWrite(fd, data, strlen(data));
		sceIoClose(fd);
	}
}

void _log_psplink(char *data) {
	SceUID fd = sceKernelStdout();
    sceIoWrite(fd, data, strlen(data));
}
