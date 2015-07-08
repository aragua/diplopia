#include <stdio.h>

#include <list.h>

int main (int argc, char ** argv)
{
	struct list * l;
	struct list_entry * e;

	l = list_new();
	if (l) {
		list_add(l,"bonjour");
		list_add_last(l,"je m'appelle");
		list_add_last(l,"Fabien");

		list_for_each(l,e) {
			printf("%s ", (char *)(e->data));
		}
		printf("\n");
		
		list_free(l);
	}
	return 0;
}
