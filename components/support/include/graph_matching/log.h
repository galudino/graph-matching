///
/// \file       log.h
/// \brief      Tagged diagnostic messages on stderr.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_LOG_H
#define GRAPH_MATCHING_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/// Print a debug message, tagged with the calling function and line.
#define logd(...) log_message("debug", __func__, __LINE__, __VA_ARGS__)

/// Report an error, tagged with the calling function and line.
#define loge(...) log_message("error", __func__, __LINE__, __VA_ARGS__)

/// Report a warning, tagged with the calling function and line.
#define logw(...) log_message("warning", __func__, __LINE__, __VA_ARGS__)

/// Print an info message, tagged with the calling function and line.
#define logi(...) log_message("info", __func__, __LINE__, __VA_ARGS__)

/// Let GCC and Clang check printf-style arguments against the format string.
#if defined(__GNUC__)
#define GRAPH_MATCHING_PRINTF_FORMAT(format_index, first_arg)                                      \
    __attribute__((format(printf, format_index, first_arg)))
#else
#define GRAPH_MATCHING_PRINTF_FORMAT(format_index, first_arg)
#endif

///
/// \brief      Print a tagged diagnostic message to stderr.
///
/// \details    Prints "[level:function:line] message". Messages longer than
///             1023 characters are truncated. Use the logd (debug), loge
///             (error), logw (warning), and logi (info) macros instead of
///             calling this directly.
///
/// \param      level       Severity tag, such as "error".
/// \param      function    Name of the reporting function.
/// \param      line        Source line of the report.
/// \param      format      printf-style format string.
///
/// \return     The number of characters written, or a negative value on error.
///
int log_message(const char *level, const char *function, int line, const char *format, ...)
    GRAPH_MATCHING_PRINTF_FORMAT(4, 5);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_LOG_H
