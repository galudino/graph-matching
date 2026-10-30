///
/// \file       log.c
/// \brief      Tagged diagnostic messages on stderr.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/log.h>

#include <stdarg.h>
#include <stdio.h>

int log_message(const char *level, const char *function, int line, const char *format, ...) {
    char message[1024];

    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof message, format, args);
    va_end(args);

    return fprintf(stderr, "[%s:%s:%d] %s", level, function, line, message);
}
