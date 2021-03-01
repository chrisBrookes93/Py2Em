#include "logging.h"

void Log(const char* pFormat, ...)
{
#if LOGGING_ON
	va_list args;
	va_start(args, pFormat);
	vfprintf(stdout, pFormat, args);
	va_end(args);
#endif
}