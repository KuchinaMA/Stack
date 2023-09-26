#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Logfile.h"

//const char *LOG_FILE = "Stackerrors";

//FILE *LOG_FILE = stderr;

static const char *EXTENSION = ".log";

void closelog(FILE *lf) {
    fprintf(lf, "--------------Файл успешно закрыт--------------\n\n\n");
    fclose(lf);
}


FILE *openlog(const char *filename) {

    char *file_name = strdup(filename); // ?
    FILE *logfile = fopen(strcat(file_name, EXTENSION), "a");

    if (logfile == NULL) {
        logfile = stderr;
        return logfile;
    }

    fprintf(logfile, "--------------Файл создан--------------\n");

    return logfile;

    //atexit(closelog);
}

