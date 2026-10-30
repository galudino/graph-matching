///
/// \file       status_code.c
/// \brief      Descriptions of status codes.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/status_code.h>

const char *status_code_string(status_code code) {
    switch (code) {
    case STATUS_OK:
        return "success";
    case STATUS_INVALID_ARGUMENT:
        return "invalid argument";
    case STATUS_OUT_OF_MEMORY:
        return "out of memory";
    case STATUS_FILE_ERROR:
        return "file error";
    case STATUS_PARSE_ERROR:
        return "malformed input";
    case STATUS_INVALID_MATCHING:
        return "invalid matching";
    }

    return "unknown status";
}
