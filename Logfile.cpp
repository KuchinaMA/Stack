#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Logfile.h"

FILE *LOG_FILE = stderr;
//char *EXTENSION = ".log";

void closelog() {
    fprintf(LOG_FILE, "Файл успешно закрыт\n");
    fclose(LOG_FILE);
}


void openlog(const char *filename) {

    char *file_name = strdup(filename);
    LOG_FILE = fopen(strcat(file_name, ".log"), "a");

    if (LOG_FILE == NULL) {
        LOG_FILE = stderr;
    }

    atexit(closelog);
}

