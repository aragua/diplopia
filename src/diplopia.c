#include <stdio.h>
#include <errno.h>

#include <diplopia.h>
#include <filesystem.h>

#define OUTPUT "/tmp/doublons/"

int print(const char * path, struct stat * statbuf)
{
	if (S_ISDIR(statbuf->st_mode))
		printf("\033[0;32;40mEntering %s\033[0m\n", path);
	else if (S_ISLNK(statbuf->st_mode))
		printf("\033[0;33;40m%s\033[0m\n", path);
	else
		printf("\033[0;31;40m%s\033[0m\n", path);
	return 0;
}

int search_duplicate(const char * path, int option)
{
	int dup = 0;

	if (!path || !is_directory(path)) {
		fprintf(stderr, "Error: %s is not a diretory\n", path);
		return -EINVAL;
	}

	parse_directory(path,option,print);

	return dup;
}
