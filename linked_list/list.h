/* $Id: list.h,v 1.1 2004/02/28 12:42:13 zwen Exp $ */

/* Implementation of simple linked lists

 * This code is in the public domain. Use it at your own risk.
 * Klaus Weidner <kweidner@pobox.com>
 */

#ifndef LIST_H
#define LIST_H

typedef void destruct_f(void *);

typedef struct list_s {
	void *val;
	int flags;
	destruct_f *destructor;
	struct list_s *next;
} List;

extern List *list_new();
extern void list_destroy(List * l);

extern List *list_insert(List * l, void *v);
extern List *list_insert_copy(List * l, void *v, int size);
extern List *list_insert_obj(List * l, int size, destruct_f * d);
extern List *list_append(List * l, void *v);
extern List *list_append_copy(List * l, void *v, int size);
extern List *list_append_obj(List * l, int size, destruct_f * d);
extern void list_remove(List * l, List * r);

/* iterate through list *l:
 *   List *p;
 *   for (p=l->next; p; p=p->next) puts(p->val);
 */

/* simple insertion sort - the user-supplied comparison function
 * gets two value pointers and should return -1, 0 or 1 for
 * a<b, a==b and a>b respectively (just like strcmp)
 * direction=1: sort upward, -1: sort downward
 * DON'T use this for large lists (>50 elements), it gets slow. */

#define LISTSORT_REVERSE 1
#define LISTSORT_UNIQ    2
typedef int compare_func(void *a, void *b);
extern void list_inssort(List * l, compare_func * cmp, int flags);

#endif				/* LIST_H */
