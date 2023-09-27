#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Logfile.h"

FILE *LOG_FILE = stderr;

void close_log() {
    fprintf(LOG_FILE, "--------------���� ������� ������--------------\n\n\n");
    fclose(LOG_FILE);
}


void open_log(const char *filename, const char *time) {

    LOG_FILE = fopen(filename, "a");

    fprintf(LOG_FILE, "--------------���� ������ � %s--------------\n", time);

    if (LOG_FILE == NULL) {
        LOG_FILE = stderr;
    }

    atexit(close_log);
}

