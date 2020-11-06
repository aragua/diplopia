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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <filesystem.h>

int is_directory(const char *path)
{
	struct stat statbuf;
	if (!path)
		return -1;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

int is_regular(const char *path)
{
	struct stat path_stat;
	if (!path)
		return -1;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

static int remove_directory_callback(const char * path, struct stat * statbuf, void * data)
{
	if (S_ISDIR(statbuf->st_mode)) {
		if (rmdir(path) != 0)
			perror("rmdir");
	} else {
		if (unlink(path) != 0)
			perror("unlink");
	}
	return 0;
}

int remove_directory(const char *path)
{
	if (!path || !is_directory(path)) {
		fprintf(stderr, "Error: %s is not a diretory\n", path);
		return -EINVAL;
	}
	parse_directory(path,
			OPT_RECURSIVE|OPT_NODOTANDDOTDOT|OPT_PARSEDIRBEFORE,
			remove_directory_callback, NULL);
	rmdir(path);
	return 0;
}

int parse_directory(const char *path, int option, int (*callback)(const char*, struct stat*, void *), void * data)
{
	DIR *d;
	size_t path_len;
	int (*statfn)(const char *pathname, struct stat *buf);

	if (!path)
		return -1;

	d = opendir(path);
	path_len = strlen(path);

	if (option&OPT_FOLLOWSYMLINK)
	  statfn = stat;
	else
	  statfn = lstat;

	if (d) {
		struct dirent *p;
		char *buf;

		buf = malloc(PATH_MAX);
		if (!buf)
			return -ENOMEM;
		else {
			while ((p=readdir(d))) {
				size_t len;
				int isdot = 0;
				struct stat statbuf;

				/* Skip . and .. to avoid to recurse on them. */
				if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				{
					isdot=1;
					if (option&OPT_NODOTANDDOTDOT)
						continue;
				}
				/* Skip hidden folder from recursion */
				if ((option&OPT_NOHIDDENFILE) &&
				    (p->d_name[0] == '.'))
					continue;

				len = path_len + strlen(p->d_name) + 2;
				if (len > PATH_MAX)
					continue;

				if (path[path_len-1] == '/')
					snprintf(buf, len, "%s%s", path, p->d_name);
				else
					snprintf(buf, len, "%s/%s", path, p->d_name);

				if (statfn(buf, &statbuf) == 0)
				{
					if (!(option&OPT_PARSEDIRBEFORE))
						callback(buf, &statbuf, data);

					if (!isdot && (option&OPT_RECURSIVE) &&	S_ISDIR(statbuf.st_mode))
					{
						/* Skip hidden folder from recursion */
						if ((option&OPT_NOHIDDENFOLDER) && (p->d_name[0] == '.'))
							continue;
						else
							parse_directory(buf, option, callback, data);
					}
					if (option&OPT_PARSEDIRBEFORE)
						callback(buf, &statbuf, data);
				}
			}
			free(buf);
		}
		closedir(d);
	}
	return 0;
}

int cp(const char * src, const char * dst)
{
	if (!src || !dst)
		return -EINVAL;

	if( access(dst, F_OK) != -1 ) {
		fprintf(stderr, "File exists!!!\n");
	} else {
		int fdout, fdin;
		struct stat statbuf;

		fdout = open(dst,O_WRONLY);
		if (fdout > 0) {
			fdin = open(src, O_RDONLY);
			if (fdin > 0) {
				if (fstat(fdin, &statbuf) == 0) {
					off_t sz = statbuf.st_size;
					unsigned char * buf = malloc(4096);
					if (buf) {
						while (sz > 0) {
							int ret = 0;
							ret = read(fdin, buf, 4096);
							sz -= ret;
							ret = write(fdout, buf, ret);
							if ( ret < 0 ) {
								fprintf(stderr, "cp fails during transfer\n");
								break;
							}
						}
					}
				}
				close(fdin);
			}
			close(fdout);
		} else {
			return fdout;  
		}
	}
	
	return -1;
}
