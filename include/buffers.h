#pragma once

void bufs_init(void);
void bufs_clean(void);
void bufs_destroy(void);
uint32_t bufs_model(char * name, uint32_t * v_count, uint32_t * vao);
uint32_t bufs_tex(char * name);
uint32_t bufs_prog(char * vsname, char * fsname);

