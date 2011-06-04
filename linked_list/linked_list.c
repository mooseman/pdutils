

/* Note - this code is from here -
http://www.kkuzba.com/links/csrc/listlink.c
*/


/*_|_| LISTLINK.C
_|_|_| Heavily commented example of using linked lists in C.
_|_|_| No warrantee or guarantee is given or implied.
_|_|_| PUBLIC DOMAIN by Kurt Kuzba. (7/3/1998)*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* *************************************************************** */
/* Note this "mygetch" function was done by "kermi3" at the following site -
* http://cboard.cprogramming.com/faq-board/27714-faq-there-getch-conio-equivalent-linux-unix.html
* Very many thanks to kermi3 for doing that!
*/
   
int mygetch( ) {
  struct termios oldt,
                 newt;
  int ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

/* *************************************************************** */

typedef struct {
void *item,
         *next,
         *prev; /* This structure provides the */
} LinkedList; /* basic framework for the list. */
                  
                  
typedef struct {
int items,
        iterator; /* This structure provides the */
LinkedList *it; /* space to store list data. */
} ListData;


LinkedList *Init_List(void)
{ /* This will initialize a list. */
LinkedList *LL = malloc(sizeof(LinkedList));
ListData *LD; /* First, reserve memory for the */
if(LL) /* list itself, and then for the */
{ /* list data, stored as item 0. */
LL->next = LL->prev = NULL;
LL->item = malloc(sizeof(ListData));
if(LL->item) /* We need to initialize first */
{ /* and last link to NULL, and */
LD = LL->item; /* set our iterator to 0 also. */
LD->items = LD->iterator = 0;
LD->it = LL;
} /* The pointer we return will be */
} /* used from now on to identify */
return LL; /* our list in function calls. */
}


int Count_List(LinkedList *LL)
{
ListData *LD = LL->item; /* Since we keep track of how */
return LD->items; /* many items we have, this is */
} /* a very simple function. */

                             
LinkedList *List_Walk(LinkedList *LL, int i)
{ /* This will return a pointer */
int walk; /* to a specific list member. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
if(i < 1 || i > LD->items) /* Test for viability of i. */
return NULL;
if(i < (LD->items / 2)) /* Decide if forward walk will */
{ /* be faster than backward walk. */
for(walk = 0; walk < i; walk++)
LL = LL->next; /* take next pointer til i. */
}
else
{
for(walk = LD->items + 1; walk > i; walk--)
LL = LL->prev; /* take prev pointer til i. */
}
return LL; /* return pointer to list member.*/
}


void *Walk_List(LinkedList *LL, int i)
{ /* This returns an item pointer. */
return List_Walk(LL, i)->item;
}


void Inc_It(LinkedList *LL)
{ /* Icrement the list iterator. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
if(LD->items > LD->iterator)/* Test for last list element. */
{
LD->it = LD->it->next; /* Set iterator to next element. */
LD->iterator++; /* Update iterator numerator. */
}
else
{
if(LD->items)
{
LD->it = LL->next; /* Wrap to first list element. */
LD->iterator = 1; /* Update iterator numerator. */
}
}
}


void Dec_It(LinkedList *LL)
{ /* Decrement the list iterator. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
if(LD->iterator > 1) /* Test for first list element. */
{
LD->it = LD->it->prev; /* Set iterator to prev element. */
LD->iterator--; /* Update iterator numerator. */
}
else
{
if(LD->items)
{
LD->it = LL->prev; /* Wrap to first list element. */
LD->iterator = LD->items;
} /* Update iterator numerator. */
}
}


int Set_It(LinkedList *LL, int i)
{ /* Set the iterator element. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
if(i <= LD->items && i > 0) /* Check bounds, set iterator. */
LD->it = List_Walk(LL, LD->iterator = i);
return LD->iterator; /* Return iterator numerator. */
}


void *Get_It(LinkedList *LL)
{ /* Get the iterator item. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
return LD->it->item; /* Return the iterator item. */
}


LinkedList *Know_It(LinkedList *LL)
{ /* Get the iterator element. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
return LD->it; /* Return the iterator element. */
}


void Swap_List(LinkedList *LL, int a, int b)
{
void *it; /* Set up a temp item pointer. */
LinkedList *at, *bt; /* Set up temp list pointers. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
if(a > 0 && a <= LD->items && b > 0 && b <= LD->items)
{ /* Perform bounds check on a & b.*/
at = List_Walk(LL, a); /* Get the pointer to element a. */
bt = List_Walk(LL, b); /* Get the pointer to element b. */
it = at->item; /* Get the item pointer from a. */
at->item = bt->item; /* Redefine item pointer for a. */
bt->item = it; /* Redefine item pointer for b. */
}
}


void Del_iList(LinkedList *LL, int i)
{ /* Remove a known list element. */
LinkedList *item = List_Walk(LL, i), *nxt, *prv;
ListData *LD = LL->item; /* Set up a ListData pointer. */
if(item) /* Test for valid list element. */
{
nxt = item->next; /* Get next list element. */
prv = item->prev; /* Get prev list element. */
free(item->item); /* Free list item. */
free(item); /* Free list element. */
if(nxt && prv) /* Test for mid-list element. */
{
nxt->prev = prv; /* Link next element to prev. */
prv->next = nxt; /* Link prev element to next. */
}
if(nxt && !prv) /* Test for first list element. */
{
LL->next = nxt; /* Link next as first element. */
nxt->prev = NULL; /* Set prev link to NULL. */
}
if(prv && ! nxt) /* Test for last list element. */
{
LL->prev = prv; /* Link prev as first element. */
prv->next = NULL; /* Set next link to NULL. */
}
if(!prv && !nxt) /* Test for only list element. */
LL->prev = LL->next = NULL;
LD->items--; /* Update list items counter. */
LD->iterator = (LD->iterator > LD->items)
? LD->items /* Update iterator numerator. */
: LD->iterator;
Set_It(LL, LD->iterator);/* Update list iterator. */
}
}


void Del_pList(LinkedList *LL, void *p)
{ /* Remove unknown list element. */
int i; /* Create an int to count with. */
ListData *LD = LL->item; /* Set up a ListData pointer. */
LinkedList *LI = LL; /* Set pointer to list pointer. */
for(i = 1; i < LD->items; i++)
{ /* Walk the list looking for the */
LI = LI->next; /* list item equal to pointer */
if(LI->item == p) /* given as an function argument.*/
{
Del_iList(LL, i); /* If and when a matching item */
break; /* is found, remove it and exit. */
}
}
}


void Erase_List(LinkedList *LL)
{ /* Remove all list elements. */
while(Count_List(LL)) /* While the list has elements. */
Del_iList(LL, 1); /* Remove the first element. */
}


void Kill_List(LinkedList *LL)
{ /* Remove all traces of a list. */
Erase_List(LL); /* Remove all list elements. */
free(LL->item); /* Free the ListData pointer. */
free(LL); /* Free the LinkedList pointer. */
}


void Add_To_List(LinkedList *LL, void *p, int i)
{ /* Add an element to the list. */
LinkedList *nxt, *prv, *added = malloc(sizeof(LinkedList));
ListData *LD = LL->item; /* Set up a ListData pointer. */
added->item = p; /* Assign item to new element. */
if(0 == LD->items) /* Test for only list element. */
{ /* Link as first and last. */
LL->next = LL->prev = added;
added->prev = added->next = NULL;
LD->items++; /* Update list counter. */
return;
}
if(i > LD->items || i < 1) /* Test for out-of-list element. */
{
prv = LL->prev; /* Link as last element and */
prv->next = LL->prev = added;
added->prev = prv; /* link to prev last element. */
added->next = NULL; /* Set next link to NULL. */
LD->items++; /* Update list counter. */
return;
} /* It must be a midlist element. */
nxt = List_Walk(LL, i); /* Find present element at i. */
prv = nxt->prev; /* Get the prev link at i. */
added->next = nxt; /* Link to next element. */
added->prev = prv; /* Link to prev element. */
prv->next = nxt->prev = added; /*Link next and prev to element.*/
LD->items++; /* Update list counter. */
Set_It(LL, LD->iterator); /* Update list iterator. */
}


int Sort_Str_List(LinkedList *LL)
{
int outer, inner, item, limit = ((ListData*)LL->item)->items;
char **Ptr_list = malloc(sizeof(char*) * limit);
if(!Ptr_list)
return 1;
Set_It(LL, 1);
for(item = 0; item < limit; item++)
{
Ptr_list[item] = Get_It(LL);
Inc_It(LL);
}
for(outer = 0; outer < limit; outer++)
{
char *keep = Ptr_list[outer];
for(inner = outer + 1; inner < limit; inner++)
if(strcmp(keep, Ptr_list[inner]) > 0)
{
keep = Ptr_list[inner];
item = inner;
}
if(keep != Ptr_list[outer])
{
Ptr_list[item] = Ptr_list[outer];
Ptr_list[outer] = keep;
}
}
Set_It(LL, 1);
for(item = 0; item < limit; item++)
{
Know_It(LL)->item = Ptr_list[item];
Inc_It(LL);
}
free(Ptr_list);
return 0;
}


/* Create a list and do various things to it. */
int main(void)
{
int line, end;
char *testline;
LinkedList *ll = NULL;
ll = Init_List();
for(line = 1; line < 20; line++)
{
testline = malloc(81);
sprintf(testline, "This is line %d, at %p.\n", line, testline);
Add_To_List(ll, testline, 0);
}
testline = malloc(81);
sprintf(testline, "%s",
"Oops!! ... How did THAT get in there?!\n");
Add_To_List(ll, testline, 9);
end = Count_List(ll);
Set_It(ll, 1);
for(line = 1; line <= end; line++)
{
printf(Get_It(ll));
Inc_It(ll);
}
puts("\n");
mygetch();
Del_pList(ll, testline);
end = Count_List(ll);
Set_It(ll, 1);
for(line = 1; line <= end; line++)
{
printf(Get_It(ll));
Inc_It(ll);
}
puts("\n");
mygetch();
Del_iList(ll, 9);
end = Count_List(ll);
Set_It(ll, 1);
for(line = 1; line <= end; line++)
{
printf(Get_It(ll));
Inc_It(ll);
}
puts("\n");
mygetch();
end = Count_List(ll);
Swap_List(ll, 1, end);
Swap_List(ll, 2, end - 1);
Set_It(ll, 1);
for(line = 1; line <= end; line++)
{
printf(Get_It(ll));
Inc_It(ll);
}
puts("\n");
mygetch();
Sort_Str_List(ll);
Set_It(ll, 1);
for(line = 1; line <= end; line++)
{
printf(Get_It(ll));
Inc_It(ll);
}
puts("\n");
mygetch();
Kill_List(ll);
return 0;
}

