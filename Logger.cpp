#include "Logger.h"

struct LOGGED_FUNCTION *Stack_trace_buffer = NULL;
int _log_init = open_log();
static int FunctionsCount = 0;

void add_func_to_log (const char *file, const char *function, int line){
    solver_assert_without_logger (file != NULL,                    pointer_is_null, (void)" ");
    solver_assert_without_logger (function != NULL,                pointer_is_null, (void)" ");
    solver_assert_without_logger (Stack_trace_buffer != NULL,      pointer_is_null, (void)" ");
    solver_assert_without_logger (strlen (file) < FILENAME_MAX,    length_too_big, (void)" ");
    solver_assert_without_logger (strlen (file) < FILENAME_MAX,    length_too_big, (void)" ");

    if (FunctionsCount >= TRACE_BUFFER_SIZE)
        return;

    strcpy (Stack_trace_buffer[FunctionsCount].file, file);
    strcpy (Stack_trace_buffer[FunctionsCount].function, function);
    Stack_trace_buffer[FunctionsCount].line = line;
    FunctionsCount++;
}

void Show_stack_trace (){
    for (int i = FunctionsCount - 1; i >= 0; i--){
        printf_color (Console_default, Console_bold, "#%d\tfunction: %-70s (in file %s:%d)\n",\
            FunctionsCount - 1 - i, Stack_trace_buffer[i].function, Stack_trace_buffer[i].file,\
            Stack_trace_buffer[i].line);
    }
}

void pop_func_from_log (){
    FunctionsCount--;
}

int open_log(){
    Stack_trace_buffer = (struct LOGGED_FUNCTION*)calloc(TRACE_BUFFER_SIZE, sizeof(LOGGED_FUNCTION));
    atexit(close_log);
    return 1;
}

void close_log(){
    free(Stack_trace_buffer);
}

