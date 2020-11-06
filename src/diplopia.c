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
		
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "diplopia.h"
#include "filesystem.h"
#include "md5.h"

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

static int add_new_path(const char * path, unsigned char * dgst, struct list * digest_list)
{
	struct list_entry * e;
	struct digest *new;
	if (!path || !dgst)
		return -EINVAL;

	/* check if the digest already exists ...*/
	list_for_each(digest_list,e)
	{
		struct digest * d = (struct digest *)(e->data);
		/* ... If yes we add the new path */
		if (memcmp(d->digest, dgst, 16)==0)
		{
			list_add(d->paths, strdup(path));
			return 0;
		}
	}

	/* ... If not we create a new instance with the digest */
	new = malloc(sizeof(struct digest));
	if (new) {
		if (digest_init(new, dgst)==0)
		{
			list_add(new->paths, strdup(path));
			list_add(digest_list,new);
			return 0;
		} else
			free(new);
	}

   	return -1;
}

/* Callback to treat all files */
static int printmd5(const char *path, struct stat *statbuf, void *data)
{
	struct list * digest_list = (struct list *)data;

	if (!path || !statbuf || !digest_list)
		return EXIT_FAILURE;

	if (S_ISDIR(statbuf->st_mode))
	{
		printf("\rParsing %s", path);
		fflush(stdout);
	}
	else if (S_ISLNK(statbuf->st_mode))
	{
		printf("\r%s is a link", path);
		fflush(stdout);
	}
	else
	{
		unsigned char digest[16];
		md5sum_path(path, digest);
		add_new_path(path, digest, digest_list);
	}

	return EXIT_SUCCESS;
}

/* flags are related to flags defined in filesystem.h */
#define RECURSIVE 1<<0
#define NOHIDDENFILE 1<<4
#define NOHIDDENFOLDER 1<<5
#define SHOWSINGLE 1<<6

int search_duplicate(struct list * paths, int option)
{
	int dup = 0;
	struct list_entry *el, *p;
	char * out;
	/* Define the list of digest and paths */
	struct list * digest_list;

	if (!paths)
		return -EINVAL;

	digest_list = list_new();
	if (!digest_list)
		return -ENOMEM;

	list_for_each(paths, p)
	{
		if (p && is_directory((char *)p->data))
			parse_directory((char *)p->data, OPT_NODOTANDDOTDOT|option, printmd5, digest_list);
	}
	printf("\r");

	out = malloc(33);
	if (out)
	{
		list_for_each(digest_list,el) {
			struct list_entry *ed;
			struct digest * d = el->data;
			int n;

			for (n = 0; n < 16; ++n)
				snprintf(&(out[n * 2]), 16 * 2, "%02x", (unsigned int)d->digest[n]);

			if (d->paths->count == 1) {
				if (option&SHOWSINGLE)
					printf("\033[0;32;40m%s\033[0m : %s\n",
					       out,
					       (char*)(d->paths->first->data));
			} else {
				printf("\033[0;31;40m%s :\033[0m\n", out);
				list_for_each(d->paths,ed) {
					printf("\t%s\n", (char*)ed->data);
				}
			}
		}
		free(out);
	}
	list_clean_data(digest_list, (void(*)(void *))digest_free);
	list_free(digest_list);
	
	return dup;
}


void Usage(void)
{
	fprintf(stderr, "diplopia [options] <folder path>\n");
	fprintf(stderr, "\noptions:\n");
	fprintf(stderr, "\t-d : Do not parse dot prefixed directories\n");
	fprintf(stderr, "\t-f : Do not treat dot prefixed files\n");
	fprintf(stderr, "\t-r : Do not parse recursively\n");
	fprintf(stderr, "\t-s : Show single file\n");
}

int main (int argc, char **argv)
{
	int opt, option = RECURSIVE, ret = EXIT_FAILURE;
	struct list * paths;

	while ((opt = getopt(argc, argv, "dfrs")) != -1) {
	    switch (opt) {
	    case 'd':
		    option |= NOHIDDENFOLDER;
		    break;
	    case 'f':
		    option |= NOHIDDENFILE;
		    break;
	    case 'r':
		    option &= ~RECURSIVE;
		    break;
	    case 's':
		    option |= SHOWSINGLE;
		    break;
	    default:
		    Usage();
		    exit(1);
	    }
	}
	if ((argc - optind) < 1) {
		Usage();
		return -EINVAL;
	}
	
	paths = list_new();
	for (size_t idx = optind; idx < argc; idx++) {
		list_add(paths, argv[optind]);
	}
	ret = search_duplicate(paths,option);

	list_free(paths);
	
	return ret;
}

