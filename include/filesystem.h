#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int is_directory(const char *path);
int is_regular(const char *path);
int remove_directory(const char *path);
int parse_directory(const char *path, int recursive, int (*callback)(const char*, struct stat*));

#define OPT_RECURSIVE 1<<0
#define OPT_NODOTANDDOTDOT 1<<1
#define OPT_PARSEDIRBEFORE 1<<2

#endif
