#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <diplopia.h>

int main (int argc, char **argv)
{
	int option = 1;

	if (argc < 2) {
		fprintf(stderr, "Error: need a folder path to parse\n");
		return -EINVAL;
	}

	if (argc >2)
		option = atoi(argv[2]);

	printf("Parsing %s\n", argv[1]);

	search_duplicate(argv[1],option);

	return 0;
}
