/**
 ******************************************************************************
 * @file    Logger.cpp
 * @date    04-10-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Logger component for application.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "SDK/UnaLogger/Logger.h"
#include "SDK/Interfaces/IKernel.hpp"

#include <stdlib.h>
#include <stdarg.h>


extern const IKernel* kernel;

void Logger_message(const char* level,
                    const char* module,
                    const char* module_sep,
                    const char* func,
                    int line,
                    const char* fmt, ...)
{

    char meta[64];
    snprintf(meta, sizeof(meta), "%s%s%s%s%d",
            module, module_sep, func, module_sep, line);

    char full_fmt[128];
    snprintf(full_fmt, sizeof(full_fmt),
             "-%s- %-36s: %s", level, meta, fmt);

    va_list args;
    va_start(args, fmt);
    kernel->app.vlog(full_fmt, args);
    va_end(args);
}


void Logger_hexdump(const char* level,
                    const char* module,
                    const char* module_sep,
                    const char* func,
                    int line,
                    const void* pData,
                    int len)
{
    if (pData == nullptr || len == 0)
        return;

    const uint8_t *p = static_cast<const uint8_t*>(pData);
    char buf[128];

    for (int i = 0; i < len; i += 16) {
        char *out = buf;
        int remaining = sizeof(buf);
        int written = 0;
#if 1
        // indent or prefix (if offset is needed)
        written = snprintf(out, remaining, "%04X: ", i);
        out += written;
        remaining -= written;
#endif
        for (int j = 0; j < 16 && (i + j) < len; j++) {
            if (j == 8) {
                *out++ = ' ';
                remaining--;
            }
            written = snprintf(out, remaining, " %02X", p[i + j]);
            out += written;
            remaining -= written;
        }

        snprintf(out, remaining, "\n");

        Logger_message(level, module, module_sep, func, line, "%s", buf);
    }
}
