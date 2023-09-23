#include "Logger.h"
#include "CustomAssert.h"

#ifdef _SHOW_STACK_TRACE

struct LOGGED_FUNCTION *Stack_trace_buffer = NULL;
int backtrace_log_init_ = open_log();
static int FunctionsCount = 0;

void add_func_to_log (const char *file, const char *function, int line){
    custom_assert_without_logger (backtrace_log_init_,                invalid_value,   (void)0);
    custom_assert_without_logger (file != NULL,                       pointer_is_null, (void)0);
    custom_assert_without_logger (function != NULL,                   pointer_is_null, (void)0);
    custom_assert_without_logger (Stack_trace_buffer != NULL,         pointer_is_null, (void)0);
    custom_assert_without_logger (strlen (file) < FILENAME_MAX,       length_too_big,  (void)0);
    custom_assert_without_logger (strlen (file) < FILENAME_MAX,       length_too_big,  (void)0);
    custom_assert_without_logger (FunctionsCount < TRACE_BUFFER_SIZE, stack_overflow,  (void)0);

    strcpy (Stack_trace_buffer[FunctionsCount].file, file);
    strcpy (Stack_trace_buffer[FunctionsCount].function, function);
    Stack_trace_buffer[FunctionsCount].line = line;
    FunctionsCount++;
}

void Show_stack_trace (){
    custom_assert_without_logger (backtrace_log_init_,        invalid_value,   (void)0);
    custom_assert_without_logger (Stack_trace_buffer != NULL, pointer_is_null, (void)0);

    for (int i = FunctionsCount - 1; i >= 0; i--){
        custom_assert_without_logger (Stack_trace_buffer [i].file     != NULL,                            pointer_is_null,     (void)0);
        custom_assert_without_logger (Stack_trace_buffer [i].function != NULL,                            pointer_is_null,     (void)0);

        fprintf_color (CONSOLE_DEFAULT, CONSOLE_BOLD, stderr, "#%d\tfunction: %-70s (in file %s:%d)\n",\
            FunctionsCount - 1 - i, Stack_trace_buffer[i].function, Stack_trace_buffer[i].file,\
            Stack_trace_buffer[i].line);
    }
}

void pop_func_from_log (){
    custom_assert_without_logger (FunctionsCount > 0, invalid_value, (void) 0);

    FunctionsCount--;
}

int open_log(){
    Stack_trace_buffer = (struct LOGGED_FUNCTION*)calloc(TRACE_BUFFER_SIZE, sizeof(LOGGED_FUNCTION));

    custom_assert_without_logger (Stack_trace_buffer != NULL, allocation_error, 0);

    atexit(close_log);
    return 1;
}

void close_log(){
    free(Stack_trace_buffer);
}

#else

void add_func_to_log (const char *file, const char *function, int line){

}

void Show_stack_trace (){

}

void pop_func_from_log (){
}

int open_log(){
    return 1;
}

void close_log(){

}
#endif
