#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <diplopia.h>
#include <filesystem.h>
#include <md5.h>

#include <list.h>

#define OUTPUT "/tmp/doublons/"

struct digest {
	unsigned char digest[16];
	struct list * paths;
};

static struct list * digest_list;

static int digest_init(struct digest *new, unsigned char * digest)
{
	if (!new || !digest)
		return -EINVAL;
	memcpy(new->digest, digest, 16*sizeof(unsigned char));
	new->paths = list_new();
	if (!new->paths)
		return -ENOMEM;
	return 0;
}

/* Callback to treat all files */
static int printmd5(const char *path, struct stat *statbuf)
{
	if (S_ISDIR(statbuf->st_mode))
		printf("\033[0;32;40mEntering %s\033[0m\n", path);
	else if (S_ISLNK(statbuf->st_mode))
		printf("\033[0;33;40m%s is a link\033[0m\n", path);
	else {
		unsigned char digest[16];
		char *out = (char *)malloc(33);
		int n;
		md5sum_path(path, digest);
		for (n = 0; n < 16; ++n) {
			snprintf(&(out[n * 2]), 16 * 2, "%02x",
				 (unsigned int)digest[n]);
		}
		printf("%s : %s\n", out, path);
		free(out);
	}
	return 0;
}

/* Main function */
int search_duplicate(const char *path, int option)
{
	int dup = 0;

	if (!path || !is_directory(path)) {
		fprintf(stderr, "Error: %s is not a diretory\n", path);
		return -EINVAL;
	}
	//remove_directory(OUTPUT);
	//mkdir(OUTPUT,0644);

	digest_list = list_new();
	if (!digest_list)
		return -ENOMEM;

	parse_directory(path, OPT_RECURSIVE | OPT_NODOTANDDOTDOT, printmd5);

	
	
	list_free(digest_list);
	
	return dup;
}
