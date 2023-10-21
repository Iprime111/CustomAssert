#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

#include "CustomAssert.h"
#include "ColorConsole.h"


#ifndef _NDEBUG

/*!
    @brief Reads three lines from the source file. Shifts reading lines if neccessary
    @param[in] source_filename Name of the source file
    @param[in] line Middle line number
    @param[out] line_shift 1 if now the given line is the first in array and 0 if not
    @return Pointer to array with three MAX_LINE_LENGTH-sized strings or NULL if error is occuried
*/
static char *read_source (const char *source_filename, unsigned int line, unsigned int *line_shift);

/*!
    @brief Opens file for reading
    @param[in] ilename Name of the file
    @return Pointer to opened file or NULL if error is occuried
*/
static FILE *open_file (const char *filename);

/*!
    @brief Gets file path to executing binary
    @return Path to binary file or NULL if error is occuried
*/
static char *get_binary_file_path ();

/*!
    @brief Gets time of the given file's last modification
    @param[in] filename Path to the file
    @return Last modification time or -1 if error is occuried
*/
static time_t get_last_modified_date (const char *filename);

/*!
    @brief Tells if source file is outdated
    @param[in] source_filename Name of the source file
    @return false if file is outdated and true if not
*/
static bool should_read_source (const char *source_filename);

/*!
    @brief Reads line from fp till it's not blank
    @param[out] str Buffer for read string
    @param[in] fp Pointer to the file
    @param[out] read_next_line true if more than one line have been read and false if not
    @return false if errors occuried and true if not
*/
static bool read_non_empty_string (char *str, FILE *fp, bool *read_next_line);

void assert_perror_custom (enum ERROR_CODE code, const char *source_path, const char* function, unsigned int line){


    #define MSG_(err_code, descr)                                                            \
        if (code & err_code){                                                                \
            fprintf_color (CONSOLE_RED, CONSOLE_NORMAL, stderr, "\n" #err_code ": " descr);  \
        }                                                                                    \

    MSG_ (undefined_variable,   "Variable is undefined")
    MSG_ (number_is_nan,        "Requested number is NaN")
    MSG_ (number_is_inf,        "Requested number is inf")
    MSG_ (not_enough_pointers,  "One or multiple pointers are similar")
    MSG_ (eof_found,            "EOF symbol has been found in input buffer")
    MSG_ (pointer_is_null,      "Requested pointer is NULL")
    MSG_ (cannot_open_file,     "Can not open file")
    MSG_ (wrong_data_format,    "Incorrect data format")
    MSG_ (invalid_arguments,    "Invalid command line arguments")
    MSG_ (file_close_error,     "Can not close file")
    MSG_ (length_too_big,       "String length is too big")
    MSG_ (invalid_value,        "Variable value is invalid")
    MSG_ (stack_overflow,       "Stack has reached it's maximum size")
    MSG_ (allocation_error,     "Error occuried while allocating memory")

    fprintf_color (CONSOLE_RED, CONSOLE_NORMAL, stderr, " in %s %s:%d\n",\
                    function, source_path, line);

    unsigned int line_shift = 0;

    char * code_lines = read_source (source_path, line, &line_shift);

    if (code_lines != NULL){
        if (line_shift){
            fprintf_color (CONSOLE_BLUE, CONSOLE_BOLD,    stderr, "%5.d|\t%s", line, code_lines);
            fprintf_color (CONSOLE_WHITE, CONSOLE_NORMAL, stderr, "     |\t%s", code_lines + MAX_LINE_LENGTH);
        }else{
            fprintf_color (CONSOLE_WHITE, CONSOLE_NORMAL, stderr, "     |\t%s", code_lines);
            fprintf_color (CONSOLE_BLUE, CONSOLE_BOLD,    stderr, "%5.d|\t%s", line, code_lines + MAX_LINE_LENGTH);
        }

        fprintf_color (CONSOLE_WHITE, CONSOLE_NORMAL, stderr,  "     |\t%s", code_lines + MAX_LINE_LENGTH * 2);

        free (code_lines);
    }


    fprintf (stderr, "\n");

    #ifdef _SHOW_STACK_TRACE
        fprintf_color (CONSOLE_RED, CONSOLE_BOLD, stderr, "STACK TRACE:\n");

        Show_stack_trace ();
    #endif

    fprintf (stderr, "\n");

    #undef MSG_
}

static FILE *open_file (const char *filename){
    FILE *fp = fopen (filename, "r");

    if (fp == NULL || ferror (fp))
        fp = NULL;

    return fp;
}

static char *get_binary_file_path (){
    char *filename_buf = (char *) calloc (FILENAME_MAX, sizeof(char));

    if (readlink ("/proc/self/exe", filename_buf, FILENAME_MAX) < 0){
        free (filename_buf);
        return NULL;
    }

    return filename_buf;
}

static time_t get_last_modified_date (const char *filename){
    struct stat stat_buf;

    if (stat (filename, &stat_buf))
        return -1;

    return stat_buf.st_mtim.tv_sec;

}

static bool should_read_source (const char *source_path){
    char *binary_path = get_binary_file_path ();

    if (!binary_path){
        fprintf_color (CONSOLE_YELLOW, CONSOLE_BOLD, stderr,\
                    "Unexpected error is occuried while reading binary file path!\n");

        return false;
    }

    time_t source_modified_time = get_last_modified_date (source_path);
    time_t binary_modified_time = get_last_modified_date (binary_path);

    bool ret_value = true;

    if (binary_modified_time < 0){
        fprintf_color (CONSOLE_YELLOW, CONSOLE_BOLD, stderr,\
                    "Unexpected error is occuried while reading binary file metadata! (%s)\n", binary_path);

        ret_value = false;
    }

    if (source_modified_time < 0){
        fprintf_color (CONSOLE_YELLOW, CONSOLE_BOLD, stderr,\
                "Unexpected error is occuried while reading source file metadata! (%s)\n", source_path);

        ret_value = false;
    }

    if (source_modified_time > binary_modified_time){
        fprintf_color (CONSOLE_YELLOW, CONSOLE_BOLD, stderr,\
                "Source file was modified after binary compilation!\n");

        ret_value = false;
    }

    free (binary_path);

    return ret_value;
}

static bool read_non_empty_string (char *str, FILE *fp, bool *read_next_line){

    *read_next_line = false;
    int read_lines = 0;
    do{
        if (read_lines)
            *read_next_line = true;
        fgets (str, MAX_LINE_LENGTH, fp);
        read_lines++;
    }while (str[0] == '\n');

    if (feof (fp) || ferror (fp) || str[0] == '\0')
        return false;

    return true;
}

static char *read_source (const char *source_path, unsigned int line, unsigned int *line_shift){
    if (!should_read_source (source_path))
        return NULL;

    FILE *fp = open_file (source_path);

    int cur_ch = getc (fp);
    unsigned int readed_lines = 0;

    while (cur_ch != EOF && readed_lines < line - 2){
        if (cur_ch == '\n')
            readed_lines++;

        cur_ch = getc (fp);
    }

    fseek (fp, -1, SEEK_CUR);

    if (cur_ch == EOF || ferror (fp)){
        fclose (fp);
        fprintf (stderr, "Error occuried while reading source file! (%s)", source_path);
        return NULL;
    }

    char *code_lines = (char *) calloc (3, sizeof (char) * MAX_LINE_LENGTH);

    *line_shift = 0;
    bool read_next_line = false;

    if (read_non_empty_string (code_lines, fp, &read_next_line)){
        *line_shift += read_next_line;
        if (read_non_empty_string (code_lines + MAX_LINE_LENGTH, fp, &read_next_line)){
            read_non_empty_string (code_lines + MAX_LINE_LENGTH * 2, fp, &read_next_line);
        }
    }

    return code_lines;
}

#endif
