#include <pspkernel.h>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

void _log_file(char *data);

void _log(char *fmt, ...) {
#if 0 == 1
	va_list list;
	char data[4096];

	va_start(list, fmt);
	vsnprintf(data, 4096, fmt, list);
	va_end(list);

	_log_file(data);
	//_log_psplink(data);
#endif
}

void _log_file(char *data) {
#if 0 == 1
	SceUID fd = sceIoOpen("ms0:/music.prx.log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
	if(fd > -1) {
		sceIoWrite(fd, data, strlen(data));
		sceIoClose(fd);
	}
#endif
}

void _log_psplink(char *data) {
#if 0 == 1
	SceUID fd = sceKernelStdout();
    sceIoWrite(fd, data, strlen(data));
#endif
}
