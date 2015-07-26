/*
  Copyright (c) 2015 Fabien Lahoudere

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

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
int cp(const char * src, const char * dst);

#define OPT_RECURSIVE 1<<0
#define OPT_NODOTANDDOTDOT 1<<1
#define OPT_PARSEDIRBEFORE 1<<2
#define OPT_FOLLOWSYMLINK 1<<3
#define OPT_NOHIDDENFILE 1<<4
#define OPT_NOHIDDENFOLDER 1<<5

#endif
