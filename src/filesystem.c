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

int remove_directory(const char *path)
{
	DIR *d;
	size_t path_len;
	int r = -1;

	if (!path)
		return -1;

	d = opendir(path);
	path_len = strlen(path);

	if (d) {
		struct dirent *p;
		r = 0;

		while (!r && (p=readdir(d))) {
			int r2 = -1;
			char *buf;
			size_t len;

			/* Skip . and .. to avoid to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);

			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode)) {
						r2 = remove_directory(buf);
					} else {
						r2 = unlink(buf);
					}
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}

	if (!r) {
		r = rmdir(path);
	}

	return r;
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

				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (option&OPT_PARSEDIRBEFORE)
						callback(buf,&statbuf);
					if (!isdot &&
						(option&OPT_RECURSIVE) &&
						S_ISDIR(statbuf.st_mode)) {
						parse_directory(buf, option, callback);
					}
					if (!(option&OPT_PARSEDIRBEFORE))
						callback(buf,&statbuf);
				}
			}
			free(buf);
		}
		closedir(d);
	}

	return 0;
}

