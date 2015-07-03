#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <diplopia.h>
#include <filesystem.h>
#include <md5.h>

#define OUTPUT "/tmp/doublons/"

static int printmd5(const char * path, struct stat * statbuf)
{
	if (S_ISDIR(statbuf->st_mode))
		printf("\033[0;32;40mEntering %s\033[0m\n", path);
	else if (S_ISLNK(statbuf->st_mode))
		printf("\033[0;33;40m%s\033[0m\n", path);
	else {
		unsigned char digest[16];
		char *out = (char*)malloc(33);
		int n;
		md5sum_path(path, digest);
		for (n = 0; n < 16; ++n) {
			snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
		}
		printf("%s : %s\n", path, out);
		free(out);
	}
	return 0;
}

int search_duplicate(const char * path, int option)
{
	int dup = 0;

	if (!path || !is_directory(path)) {
		fprintf(stderr, "Error: %s is not a diretory\n", path);
		return -EINVAL;
	}
	//remove_directory(OUTPUT);
	//mkdir(OUTPUT,0644);

	parse_directory(path,OPT_RECURSIVE|OPT_NODOTANDDOTDOT,printmd5);

	return dup;
}
