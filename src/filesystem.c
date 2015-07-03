#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

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


static int remove_directory_callback(const char * path, struct stat * statbuf)
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

	parse_directory(path, OPT_RECURSIVE|OPT_NODOTANDDOTDOT|OPT_PARSEDIRBEFORE,remove_directory_callback);

	rmdir(path);

	return 0;
}

int parse_directory(const char *path, int option, int (*callback)(const char*, struct stat*))
{
	DIR *d;
	size_t path_len;

	if (!path)
		return -1;

	d = opendir(path);
	path_len = strlen(path);

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
				if (!strcmp(p->d_name, ".") ||
					!strcmp(p->d_name, "..")) {
					isdot=1;
					if (option&OPT_NODOTANDDOTDOT)
						continue;
				}

				len = path_len + strlen(p->d_name) + 2;
				if (len > PATH_MAX)
					continue;

				if (path[path_len-1] == '/')
					snprintf(buf, len, "%s%s", path, p->d_name);
				else
					snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (!(option&OPT_PARSEDIRBEFORE))
						callback(buf,&statbuf);
					if (!isdot &&
						(option&OPT_RECURSIVE) &&
						S_ISDIR(statbuf.st_mode)) {
						parse_directory(buf, option, callback);
					}
					if (option&OPT_PARSEDIRBEFORE)
						callback(buf,&statbuf);
				}
			}
			free(buf);
		}
		closedir(d);
	}

	return 0;
}

