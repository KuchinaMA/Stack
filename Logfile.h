#ifndef LOGFILE_H_INCLUDED
#define LOGFILE_H_INCLUDED

#include <stdio.h>

extern FILE *LOG_FILE;

/// Closes logfile
void close_log();
/**Opens logfile
 * \param [in] filename name of logfile
 * \param [in] time time when logfile was opened
*/
void open_log(const char *filename, const char *time);

#endif // LOGFILE_H_INCLUDED
