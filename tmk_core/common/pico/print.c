
#include "sendchar.h"
#include <stdarg.h>
#include <stdio.h>

int xputc(int c) {
    return sendchar((uint8_t)c);
}

int xprintf(const char *fmt, ...) {
    char buffer[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len <= 0) {
        return len;
    }
    if (len > (int)sizeof(buffer)) {
        len = sizeof(buffer);
    }

    for (int i = 0; i < len; i++) {
        sendchar((uint8_t)buffer[i]);
    }

    return len;
}
