/* $Id: list.c,v 1.1 2004/02/28 12:42:13 zwen Exp $ */

/* Simple linked lists

 * This code is in the public domain. Use it at your own risk.
 * Klaus Weidner <kweidner@pobox.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h> 
#include "list.h"

#define FL_FREE 1
#define FL_DESTRUCTOR 2

extern void* malloc(size_t s);

List *list_new()
{
	List *l;

	l = malloc(sizeof(List));

	l->val = 0;
	l->next = 0;
	l->flags = 0;

	return l;
}

List *list_insert(List * l, void *v)
{
	List *nl;

	nl = malloc(sizeof(List));

	nl->val = v;
	nl->next = l->next;
	nl->flags = 0;
	l->next = nl;

	return nl;
}

List *list_append(List * l, void *v)
{
	List *nl;

	nl = malloc(sizeof(List));

	while (l->next)
		l = l->next;

	nl->val = v;
	nl->next = l->next;
	nl->flags = 0;
	l->next = nl;

	return nl;
}

void list_freenode(List * r)
{
	if (r->flags & FL_FREE)
		free(r->val);
	if (r->flags & FL_DESTRUCTOR) {
		r->destructor(r->val);
		free(r->val);
	}
	free(r);
}

void list_remove(List * l, List * r)
{
	while (l && l->next != r)
		l = l->next;
	if (!l)
		return;
	l->next = r->next;
	list_freenode(r);
}

List *list_insert_copy(List * l, void *v, int size)
{
	void *nv;
	List *r;

	nv = malloc(size);

	if (v)
		memcpy(nv, v, size);

	r = list_insert(l, nv);
	if (!r)
		return 0;
	r->flags = FL_FREE;

	return r;
}

List *list_append_copy(List * l, void *v, int size)
{
	void *nv;
	List *r;

	nv = malloc(size);

	if (v)
		memcpy(nv, v, size);

	r = list_append(l, nv);
	if (!r)
		return 0;
	r->flags = FL_FREE;

	return r;
}

List *list_insert_obj(List * l, int size, destruct_f * destr)
{
	void *nv;
	List *r;

	nv = malloc(size);

	r = list_insert(l, nv);
	if (!r)
		return 0;
	r->flags = FL_DESTRUCTOR;
	r->destructor = destr;

	return r;
}

List *list_append_obj(List * l, int size, destruct_f * destr)
{
	void *nv;
	List *r;

	nv = malloc(size);

	r = list_append(l, nv);
	if (!r)
		return 0;
	r->flags = FL_DESTRUCTOR;
	r->destructor = destr;

	return r;
}

void list_destroy(List * r)
{
	while (r) {
		List *fl;

		fl = r->next;
		list_freenode(r);
		r = fl;
	}
}

#ifdef DON_T_USE_THIS_ITS_BROKEN
/* simple insertion sort for linked list - don't use this
 * for large lists (>50 elements), since it's inefficient */
void list_inssort(List * l, compare_func * cmpf, int flags)
{
	List *p;
	List new;
	int direction = 1;
	int uniq = 0;

	if (flags & LISTSORT_REVERSE)
		direction = -1;
	if (flags & LISTSORT_UNIQ)
		uniq = 1;

	new.next = 0;
	p = l->next;
	while (p) {
		List *nextp = p->next;
		List *q;
		q = &new;
		while (q) {
			List *nextq = q->next;
			if (nextq) {
				int cmpres;

				cmpres = direction * cmpf(p->val, nextq->val);

				if (cmpres < 0) {
					q->next = p;
					p->next = nextq;
					break;
#if 0
				} else if (cmpres == 0 && uniq) {
					/* we've already got this - remove */
					list_freenode(p);
					goto next_p;
#endif
				}
			} else {
				q->next = p;
				p->next = 0;
			}
			q = nextq;
		}
		p = nextp;
	}
	l->next = new.next;
}
#endif



#include <string.h>

typedef struct {
	char *string1;
	char *string2;
	int dummy;
} test_data_t;

void test_destructor(void *p)
{
	test_data_t *this = p;

	printf("Test destructor, string2=%s\n", this->string2);
	free(this->string1);
	free(this->string2);
}

void obj_test(void)
{
	List *l, *p;
	test_data_t *obj;

	l = list_new();
	list_insert(l, "foobar");

	p = list_insert_obj(l->next, sizeof(test_data_t), test_destructor);
	obj = p->val;
	obj->string1 = malloc(100);
	obj->string2 = strdup("Test");
	obj->dummy = 12;


	p = list_append_obj(l->next, sizeof(test_data_t), test_destructor);
	obj = p->val;
	obj->string1 = malloc(10);
	obj->string2 = strdup("hello world");
	obj->dummy = 10;

	list_append_copy(l, "baz", 4);

	list_destroy(l);
}

int cmpnum(void *ap, void *bp)
{
	int a, b;
	a = *(int *) ap;
	b = *(int *) bp;
	if (a < b)
		return -1;
	return a == b ? 0 : 1;
}

int main()
{
	List *l, *p;
	int val, i;

	l = list_new();
	list_insert(l, "d-first");
	list_insert(l, "c-second");
	list_insert_copy(l, "b-third", 8);
	list_insert(l, "a-fourth");
	list_append(l, "e-last");
	p = list_insert(l->next, "f-to-be-deleted");
	list_remove(l, p);
	p = list_insert(l, "g-to-be-deleted");
	list_remove(l, p);

	for (p = l->next; p; p = p->next) {
		printf("val='%s'\n", (char *) p->val);
	}
	puts("");

	list_inssort(l, strcmp, LISTSORT_REVERSE);
	for (p = l->next; p; p = p->next) {
		printf("val='%s'\n", (char *) p->val);
	}
	puts("");

	list_inssort(l, strcmp, 0);
	for (p = l->next; p; p = p->next) {
		printf("val='%s'\n", (char *) p->val);
	}
	puts("");

	list_destroy(l);

	l = list_new();
	val = 105;
	list_insert_copy(l, &val, sizeof(int));
	val = 104;
	list_insert_copy(l, &val, sizeof(int));
	val = 102;
	list_insert_copy(l, &val, sizeof(int));
	val = 101;
	list_insert_copy(l, &val, sizeof(int));
	val = 103;
	list_insert_copy(l->next->next, &val, sizeof(int));
	val = 106;
	list_append_copy(l, &val, sizeof(int));
	for (p = l->next; p; p = p->next) {
		printf("val=%d\n", *(int *) p->val);
	}
	list_destroy(l);
	puts("");

	l = list_new();
	srand(time());
	for (i = 0; i < 50; i++) {
		val = rand() % 10;
		list_insert_copy(l, &val, sizeof(int));
	}
	list_inssort(l, cmpnum, LISTSORT_UNIQ);
	for (p = l->next; p; p = p->next) {
		printf("val=%010d\n", *(int *) p->val);
	}
	list_destroy(l);

	l = list_new();
	list_inssort(l, cmpnum, LISTSORT_UNIQ);
	list_destroy(l);

	obj_test();

	return 0;
}


