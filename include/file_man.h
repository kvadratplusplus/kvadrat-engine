#pragma once

#include <stdio.h>

#include <defines.h>

int open_file(FILE ** file, char * name, char * fopen_mode);
void extract_directory(const char* full_path, char* dir_buffer, size_t buf_size);
void load_model(char *name, float *array, Model *model);
