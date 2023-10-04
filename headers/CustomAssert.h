#ifndef CUSTOM_ASSERT_H_
#define CUSTOM_ASSERT_H_

#include "Logger.h"
#include <time.h>

const int MAX_LINE_LENGTH = 300;

/// @brief Code of detected error
enum ERROR_CODE{
    undefined_variable  = 1 << 0,  ///< Undefined error code
    number_is_nan       = 1 << 1,  ///< Given number has NaN value
    number_is_inf       = 1 << 2,  ///< Given numver has inf value
    not_enough_pointers = 1 << 3,  ///< One or multiple pointers have one address
    eof_found           = 1 << 4,  ///< EOF symbol has been found in input buffer
    pointer_is_null     = 1 << 5,  ///< Given pointer has NULL value
    cannot_open_file    = 1 << 6,  ///< Error is occuried while opening file
    wrong_data_format   = 1 << 7,  ///< File data have wrong format
    invalid_arguments   = 1 << 8,  ///< Too many console line arguments have been passed
    file_close_error    = 1 << 9,  ///< Error is occuried while closing file
    length_too_big      = 1 << 10, ///< String length is too big
    invalid_value       = 1 << 11, ///< Invalid variable value
    stack_overflow      = 1 << 12, ///< Stack has reached it's maximum size
    allocation_error    = 1 << 13  ///< Memory allocationerror
};


#ifndef _NDEBUG

/*! @brief Custom assert implementation that provides information about file, function and line, do not stops the program and do not calls PopLog()*/
#define custom_assert_without_logger(EXP, CODE, RET) \
    custom_assert_internal(EXP, CODE, RET, {})

/*! @brief Custom assert implementation that provides information about file, function and line and do not stops the program*/
#define custom_assert(EXP, CODE, RET) \
    custom_assert_internal(EXP, CODE, RET, PopLog())

/*! @brief Custom assert implementation that provides information about file, function and line, do not stops the program and running the CALLBACK expression*/
#define custom_assert_internal(EXP, CODE, RET, CALLBACK)                                        \
            do{                                                                                 \
                if (!(EXP)){                                                                    \
                    assert_perror_custom (CODE, __FILE__, __PRETTY_FUNCTION__, __LINE__);       \
                    CALLBACK;                                                                   \
                    return RET;                                                                 \
                }                                                                               \
            }while(0)

/*!
    @brief Shows error in console
    @param[in] code Error code
    @param[in] expr Tested expression
    @return Nothing
    Shows detailed error description, it's file and line and tested expression
*/
void assert_perror_custom (enum ERROR_CODE code, const char *source_path, const char *function, unsigned int line);

/*!
    @brief Reads three lines from the source file. Shifts reading lines if neccessary
    @param[in] source_filename Name of the source file
    @param[in] line Middle line number
    @param[out] line_shift 1 if now the given line is the first in array and 0 if not
    @return Pointer to array with three MAX_LINE_LENGTH-sized strings or NULL if error is occuried
*/
char *read_source (const char *source_filename, unsigned int line, unsigned int *line_shift);

/*!
    @brief Opens file for reading
    @param[in] ilename Name of the file
    @return Pointer to opened file or NULL if error is occuried
*/
FILE *open_file (const char *filename);

/*!
    @brief Gets file path to executing binary
    @return Path to binary file or NULL if error is occuried
*/
char *get_binary_file_path ();

/*!
    @brief Gets time of the given file's last modification
    @param[in] filename Path to the file
    @return Last modification time or -1 if error is occuried
*/
time_t get_last_modified_date (const char *filename);

/*!
    @brief Tells if source file is outdated
    @param[in] source_filename Name of the source file
    @return false if file is outdated and true if not
*/
bool should_read_source (const char *source_filename);

/*!
    @brief Reads line from fp till it's not blank
    @param[out] str Buffer for read string
    @param[in] fp Pointer to the file
    @param[out] read_next_line true if more than one line have been read and false if not
    @return false if errors occuried and true if not
*/
bool read_non_empty_string (char *str, FILE *fp, bool *read_next_line);

#else
    #define custom_assert_without_logger(EXP, CODE, RET) ;
    #define custom_assert(EXP, CODE, RET) ;
    #define custom_assert_internal(EXP, CODE, RET, CALLBACK) ;

#endif
#endif

