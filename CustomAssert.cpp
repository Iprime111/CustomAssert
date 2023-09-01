#include "CustomAssert.h"

void assert_perror_custom (enum ERROR_CODE code, const char *source_path, const char* function, unsigned int line){


    #define MSG_(err_code, descr)                                                   \
        if (code & err_code){                                                       \
            printf_color (Console_red, Console_normal, "\n" #err_code ": " descr);  \
        }                                                                           \

    MSG_ (undefined_variable,   "Variable is undefined")
    MSG_ (not_an_equation,      "Folowing expression is not an equation")
    MSG_ (number_is_nan,        "Requested number is NaN")
    MSG_ (number_is_inf,        "Requested number is inf")
    MSG_ (not_enough_pointers,  "One or multiple pointers are similar")
    MSG_ (eof_found,            "EOF symbol has been found in input buffer")
    MSG_ (pointer_is_null,      "Requested pointer is NULL")
    MSG_ (exit_status_found,    "Exit status found")
    MSG_ (cannot_open_file,     "Can not open file for reading")
    MSG_ (wrong_data_format,    "Incorrect file data format")
    MSG_ (too_many_arguments,   "Too many command line arguments")
    MSG_ (file_close_error,     "Can not close file")
    MSG_ (length_too_big,       "String length is too big")

    printf_color (Console_red, Console_normal, "%s ", msg);


    #ifdef _DEBUG
        printf_color (Console_red, Console_normal, "in %s %s:%d\n",\
                        function, source_path, line);

        unsigned int line_shift = 0;

        char * code_lines = read_source (source_path, line, &line_shift);

        if (code_lines != NULL){
            if (line_shift){
                printf_color (Console_blue, Console_bold, "%5.d|\t%s", line, code_lines);
                printf_color (Console_white, Console_normal,   "     |\t%s", code_lines + MAX_LINE_LENGTH);
            }else{
                printf_color (Console_white, Console_normal, "     |\t%s", code_lines);
                printf_color (Console_blue, Console_bold,   "%5.d|\t%s", line, code_lines + MAX_LINE_LENGTH);
            }

            printf_color (Console_white, Console_normal, "     |\t%s", code_lines + MAX_LINE_LENGTH * 2);

            free (code_lines);
        }


        printf ("\n");

        #ifdef _SHOW_STACK_TRACE
            printf_color (Console_red, Console_bold, "STACK TRACE:\n");

            Show_stack_trace ();
        #endif

        printf ("\n");
    #endif

    #undef MSG_
}

FILE *open_file (const char *filename){
    FILE *fp = fopen (filename, "r");

    if (fp == NULL || ferror (fp))
        fp = NULL;

    return fp;
}

char *get_binary_file_path (){
    char *filename_buf = (char *) calloc (FILENAME_MAX, sizeof(char));

    readlink ("/proc/self/exe", filename_buf, FILENAME_MAX);

    return filename_buf;
}

time_t get_last_modified_date (const char *filename){
    struct stat stat_buf;

    if (stat (filename, &stat_buf))
        return -1;

    return stat_buf.st_mtim.tv_sec;

}

bool should_read_source (const char *source_path){
    char *binary_path = get_binary_file_path ();

    time_t source_modified_time = get_last_modified_date (source_path);
    time_t binary_modified_time = get_last_modified_date (binary_path);

    bool ret_value = true;

    if (binary_modified_time < 0){
        printf_color (Console_yellow, Console_bold,\
                    "Unexpected error is occuried while reading binary file metadata! (%s)\n", binary_path);

        ret_value = false;
    }

    if (source_modified_time < 0){
        printf_color (Console_yellow, Console_bold,\
                "Unexpected error is occuried while reading source file metadata! (%s)\n", source_path);

        ret_value = false;
    }

    if (source_modified_time > binary_modified_time){
        printf_color (Console_yellow, Console_bold,\
                "Source file was modified after binary compilation!\n");

        ret_value = false;
    }

    free (binary_path);

    return ret_value;
}

bool read_non_empty_string (char *str, FILE *fp, bool *read_next_line){

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

char *read_source (const char *source_path, unsigned int line, unsigned int *line_shift){
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
        printf ("Error occuried while reading source file! (%s)", source_path);
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
