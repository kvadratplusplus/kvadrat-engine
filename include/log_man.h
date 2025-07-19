#pragma once

enum log_type {LOG_ERROR, LOG_WARNING, LOG_LOG};

void log_init(void);
void log_close(void);
void log_log_(enum log_type type, const char function_name[], const char* format, ...);

#define log_log(type, message, ...) log_log_(type, __func__, message, __VA_ARGS__)
