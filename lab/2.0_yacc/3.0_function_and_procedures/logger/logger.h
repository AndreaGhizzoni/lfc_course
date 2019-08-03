#ifndef LOGGER_H
#define LOGGER_H

#define SIZE 1024
#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

void            write_log(char *nome_log, char *buffer);
char           *logInt(char *, int);
char           *logString(char *, char *);
char           *logString2(char *, char *, char *);
char           *logIndent(int, char *);
#endif				/* LOGGER_H */
