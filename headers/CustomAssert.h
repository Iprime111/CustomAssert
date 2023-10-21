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


#else
    #define custom_assert_without_logger(EXP, CODE, RET) ;
    #define custom_assert(EXP, CODE, RET) ;
    #define custom_assert_internal(EXP, CODE, RET, CALLBACK) ;

#endif
#endif

