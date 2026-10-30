///
/// \file       status_code.h
/// \brief      Result codes returned by library functions.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_STATUS_CODE_H
#define GRAPH_MATCHING_STATUS_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Outcome of a library call that can fail.
///
/// \details    Library functions return a status code instead of exiting,
///             so the caller decides how to report a failure.
///
typedef enum status_code {
    STATUS_OK = 0,           ///< Success.
    STATUS_INVALID_ARGUMENT, ///< An argument was out of range.
    STATUS_OUT_OF_MEMORY,    ///< An allocation failed.
    STATUS_FILE_ERROR,       ///< A file could not be opened, read, or written.
    STATUS_PARSE_ERROR,      ///< A file's contents were malformed.
    STATUS_INVALID_MATCHING, ///< A matching failed validation.
} status_code;

///
/// \brief      Describe a status code.
///
/// \param      code        Status code to describe.
///
/// \return     A short, static, human-readable description.
///
const char *status_code_string(status_code code);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_STATUS_CODE_H
