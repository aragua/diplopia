#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <diplopia.h>
#include <filesystem.h>
#include <md5.h>

#include <list.h>

/* Define the list of digest and paths */
static struct list * digest_list;

/* Define the structure where paths with same md5 are stored */ 
struct digest {
	unsigned char digest[16];
	struct list * paths;
};

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

static void digest_free(struct digest *d)
{
	if (d) {
		list_clean_data(d->paths, free);
		list_free(d->paths);
		free(d);
	}
}

static int add_new_path(const char * path, unsigned char * dgst)
{
	struct list_entry * e;
	struct digest *new;
	if (!path || !dgst)
		return -EINVAL;

	/* check if the digest already exists ...*/
	list_for_each(digest_list,e) {
		struct digest * d = (struct digest *)(e->data);
		/* ... If yes we add the new path */
		if (memcmp(d->digest, dgst, 16)==0) {
			list_add(d->paths, strdup(path));
			return 0;
		}
	}

	/* ... If not we create a new instance with the digest */
	new = malloc(sizeof(struct digest));
	if (new) {
		if (digest_init(new, dgst)==0) {
			list_add(new->paths, strdup(path));
			list_add(digest_list,new);
			return 0;
		} else
			free(new);
	}
       	return -1;
}

/* Callback to treat all files */
static int printmd5(const char *path, struct stat *statbuf)
{
	if (S_ISDIR(statbuf->st_mode)) {
		printf("\rParsing %s                                  ", path); // Fix: replace space
		fflush(stdout);
	}
	else if (S_ISLNK(statbuf->st_mode)) {
		printf("\r%s is a link                                ", path); // Fix: replace space
		fflush(stdout);
	}
	else {
		unsigned char digest[16];
		md5sum_path(path, digest);
		add_new_path(path,digest);
	}
	return 0;
}

/* Main function */
int search_duplicate(const char *path, int option)
{
	int dup = 0;
	struct list_entry *el;
	char * out;

	if (!path || !is_directory(path)) {
		fprintf(stderr, "Error: %s is not a diretory\n", path);
		return -EINVAL;
	}
	//remove_directory(OUTPUT);
	//mkdir(OUTPUT,0644);

	digest_list = list_new();
	if (!digest_list)
		return -ENOMEM;

	parse_directory(path, option, printmd5);

	printf("\n");

	out = malloc(33);
	if (out) {
		list_for_each(digest_list,el) {
			struct list_entry *ed;
			struct digest * d = el->data;
			int n;
			for (n = 0; n < 16; ++n) {
				snprintf(&(out[n * 2]), 16 * 2, "%02x",
					 (unsigned int)d->digest[n]);
			}
			if (d->paths->count == 1)
				printf("\033[0;32;40m%s :\033[0m\n", out);
			else
				printf("\033[0;31;40m%s :\033[0m\n", out);
			list_for_each(d->paths,ed) {
				printf("\t%s\n", (char*)ed->data);
			}
		}
		free(out);
	}
	list_clean_data(digest_list, (void(*)(void *))digest_free);
	list_free(digest_list);
	
	return dup;
}


int main (int argc, char **argv)
{
	int option = OPT_RECURSIVE | OPT_NODOTANDDOTDOT;

	if (argc < 2) {
		fprintf(stderr, "Error: need a folder path to parse\n");
		return -EINVAL;
	}

	if (argc > 2)
		option = atoi(argv[2]);

	printf("Parsing %s\n", argv[1]);

	search_duplicate(argv[1],option);

	return 0;
}

