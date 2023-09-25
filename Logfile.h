#ifndef LOGFILE_H_INCLUDED
#define LOGFILE_H_INCLUDED

#include <stdio.h>

extern FILE *LOG_FILE;

void closelog();
void openlog(const char *filename);

#endif // LOGFILE_H_INCLUDED
