#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//for PATH_MAX
#ifdef __WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <linux/limits.h>
#endif

#include "log_man.h"

extern char * cwd;

int open_file(FILE ** file, char * name, char * fopen_mode)
{
    char full_name[PATH_MAX];
    snprintf(full_name, PATH_MAX, "%s/%s", cwd, name);

    *file = fopen(full_name, fopen_mode);
    if (!*file) {
        log_log(LOG_ERROR, "Failed to open file \"%s\" (\"%s\")", name, full_name);
        *file = NULL;
        return 0;       //fail
    }

    return 1;       //success
}
