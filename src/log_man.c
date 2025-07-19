#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include <defines.h>
#include <log_man.h>
#include <file_man.h>

extern Config main_config;

static FILE * log_file;

void log_close(void)
{
    log_log(LOG_LOG, "Shutting down", NULL);
    fclose(log_file);
}

void log_init(void)
{
    if (!open_file(&log_file, "log.log", "a")) {
        fprintf(stderr, TEXT_ERROR " Failed to open log file\n");
        main_config.log = 0;
    } else {
        fputc('\n', log_file);
    }
    atexit(log_close);
}

void log_log_(enum log_type type, const char function_name[], const char* format, ...)
{
    va_list args;
    va_start(args, format);

    if (type == LOG_ERROR) {
        fprintf(stderr, TEXT_ERROR "  ");
        fprintf(stderr, "%-20s: ", function_name);
        vfprintf(stderr, format, args);
        fputc('\n', stderr);
    }
    if (type == LOG_WARNING) {
        fprintf(stderr, TEXT_WARNING);
        fprintf(stderr, "%-20s: ", function_name);
        vfprintf(stderr, format, args);
        fputc('\n', stderr);
    }
    if (main_config.log) {
        time_t current_time;
        struct tm *local_time;
    
        current_time = time(NULL);
        local_time = localtime(&current_time);

        fprintf(
            log_file,
            "%d-%02d-%02d %02d:%02d:%02d: ",
            local_time->tm_year + 1900,
            local_time->tm_mon + 1,
            local_time->tm_mday,
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec
        );

        if (type == LOG_ERROR)
            fprintf(log_file, "ERR: ");

        if (type == LOG_WARNING)
            fprintf(log_file, "Wrn: ");

        if (type == LOG_LOG)
            fprintf(log_file, "log: ");

        fprintf(log_file, "%-20s: ", function_name);
        vfprintf(log_file, format, args);
        fputc('\n', log_file);
    }
    va_end(args);
}
