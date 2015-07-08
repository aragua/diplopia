#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <errno.h>

struct list_entry {
	struct list_entry * previous;
	struct list_entry * next;
	void * data;
};

struct list {
	int count;
	int options;
	struct list_entry *first;
	struct list_entry *last;
};

#define LIST_OPT_MUTEX 1<<0
#define LIST_OPT_FREE_DATA 1<<1

static inline struct list_entry * list_entry_new(void * d)
{
	struct list_entry * e;
	if (!d)
		return NULL;
	e = malloc(sizeof(struct list_entry));
	if (e) {
		e->previous = NULL;
		e->next = NULL;
		e->data = d;
	}
	return e;
}
static inline void list_entry_free(struct list_entry * e)
{
	if (e)
		free(e);
}

static inline struct list * list_new(void)
{
	struct list * l;
	l = malloc(sizeof(struct list));
	if (l) {
		l->count = 0;
		l->options = 0;
		l->first = NULL;
		l->last = NULL;
	}
	return l;
}

static inline int list_insert_first(struct list * list, struct list_entry * entry)
{
	if (!list || !entry)
		return -EINVAL;
	entry->previous = NULL;
	entry->next = list->first;
	if (list->first)
		list->first->previous = entry;
	else
		list->last = entry;
       	list->first = entry;
	list->count++;
	return 0;
}

static inline int list_insert_last(struct list * list, struct list_entry * entry)
{
	if (!list || !entry)
		return -EINVAL;
	entry->next = NULL;
	entry->previous = list->last;
	if (list->last)
		list->last->next = entry;
	else
		list->first = entry;
       	list->last = entry;
	list->count++;
	return 0;
}

static inline int list_add_first(struct list * l, void * d)
{
	struct list_entry * e;
	int ret = 0;
	if (!l || !d)
		return -EINVAL;
	e = list_entry_new(d);
	if (!e)
		return -ENOMEM;
	ret = list_insert_first(l,e);
	if (ret < 0)
		free(e);
	return ret;
}

static inline int list_add_last(struct list * l, void * d)
{
	struct list_entry * e;
	int ret = 0;
	if (!l || !d)
		return -EINVAL;
	e = list_entry_new(d);
	if (!e)
		return -ENOMEM;
		ret = list_insert_last(l,e);
	if (ret < 0)
		free(e);
	return ret;
}

static inline int list_add(struct list * l, void * d)
{
	return list_add_first(l, d);
}

#define list_for_each(l,e)				\
	for (e = l->first; e ; e = e->next)

#define list_for_each_safe(l,e,n)				\
	for (e = l->first; e && ({ n=e->next; 1;}); e = n)

static inline void list_clean_data(struct list * l, void(*clean_cb)(void *))
{
	if (l) {
		struct list_entry * e, * n;
		list_for_each_safe(l,e,n) {
			clean_cb(e->data);
			e->data = NULL;
		}
	}
}

static inline int list_rm_first(struct list * l)
{
	struct list_entry * entry;
	if (!l)
		return -EINVAL;
	if (l->first) {
		entry = l->first;
		l->first = entry->next;
		if (l->options&LIST_OPT_FREE_DATA)
			free(entry->data);
		free(entry);
		if (l->first)
			l->first->previous = NULL;
		l->count--;
	}
	return 0;
}

static inline int list_rm_last(struct list * l)
{
	struct list_entry * entry;
	if (!l)
		return -EINVAL;
	if (l->last) {
		entry = l->last;
		l->last = entry->previous;
		if (l->options&LIST_OPT_FREE_DATA)
			free(entry->data);
		free(entry);
		if (l->last)
			l->last->next = NULL;
		l->count--;
	}
	return 0;
}

static inline void list_free(struct list * l)
{
	if (l) {
		struct list_entry * e, * n;
		list_for_each_safe(l,e,n)
			list_rm_first(l);
		free(l);
	}
}


#endif /* LIST_H */
