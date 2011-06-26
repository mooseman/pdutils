
/*  This code is from here -  */ 
/*  http://adamhooper.com/code?path=mcgill-se  */ 
/*  %2FCOMP206%2Fass2%2Fproducts.c             */ 
/*  It is "public domain".  */ 


/*
 * Manipulate the "products" list.
 *
 * Products are stored as follows:
 * ID,NAME,IN_STOCK,PRICE,DISCOUNT
 *
 * These are loaded into a linked list.
 *
 * Possible actions:
 * - new (give ID, name, in_stock, price, discount; check ID for duplicates)
 * - edit (give ID, name, in_stock, price, discount; check ID for existence)
 * - view (give ID)
 * - delete (give ID)
 * - list
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LINE_BUFFER_SIZE 1024
#define INPUT_BUFFER_SIZE 64
#define NUM_OUTPUT_ROWS 24
#define PRODUCTS_DB_FILE "../../data/inventory.txt"

/*
 * Rationale behind design decisions:
 *
 * Ideally, this code would be in multiple files: one for a linked list
 * implementation, one for a Product implementation, and one for the user
 * interface. I've stuffed the linked list and product stuff at the top of the
 * file.
 *
 * I chose to use a linked list because it's the proper tool for the job. Just
 * because the tool wasn't *available* to me didn't mean I couldn't hack up a
 * quick, barely-functional one. I pass that list to a lot of functions, always
 * named "products".
 *
 * All my prompts are done with "fgets (input, INPUT_BUFFER_SIZE, stdin);".
 * This is because getchar() and scanf() will annoyingly refuse to skip the
 * '\n' at the end of lines. The behavior I got with fgets() was satisfying
 * enough, though I'd have liked to use the readline library instead. I figured
 * we wouldn't be allowed.
 *
 * The prompts take up a lot of code, but they behave properly, so it's worth
 * it. There's some duplicated code in each function, mostly because I figured
 * repeating the code a few times is more understandable than 5 tiny layers of
 * abstraction.
 *
 * I don't print one more word than is asked from the assignment. I don't
 * believe in excess UI, and I don't believe in excess text. It's explained
 * better at http://www.faqs.org/docs/artu/ch11s09.html.
 *
 * My indenting and spacing style, while it may seem excessive at first, is
 * what I'm used to from hacking at GNOME. 8-space tabs, every { and } on its
 * own line. I realize it looks like a waste of space at first, but it's easy
 * enough to get used to. Hey, at least I'm consistent.
 *
 * And I go out of my way to avoid segmentation faults. Force of habit. I can't
 * bring myself to write "gets()" or call a malloc() without a free(). I ran
 * Valgrind on this program, too. No memory leaks.
 *
 * As for commenting: I comment just enough. The linked list stuff shouldn't
 * need explaining. If a function has a strange behavior that its name, return
 * value and parameters don't imply, I comment it. Otherwise, I don't. The code
 * should speak for itself.
 */

/* Linked list implementation */
typedef struct _List List;

struct _List {
	void *data;
	List *next;
};

typedef void (*ListFunc) (void *data);
#define LIST_FUNC(f) ((ListFunc) (f))

static List *
list_malloc (void)
{
	return (List *) malloc (sizeof (List));
}

List *
list_prepend (List *list,
	      void *data)
{
	List *ret = list_malloc ();
	ret->data = data;
	ret->next = list;
	return ret;
}

List *
list_append (List *list,
	     void *data)
{
	List *ret;

	if (list == NULL) return list_prepend (list, data);

	ret = list;

	for (; list->next; list = list->next);
	list->next = list_malloc ();
	list->next->data = data;
	list->next->next = NULL;

	return ret;
}

List *
list_reverse (List *list)
{
	List *prev = NULL;
	List *next;

	while (list)
	{
		next = list->next;

		list->next = prev;

		prev = list;
		list = next;
	}

	return prev;
}

List *
list_remove (List *list,
	     const void *data)
{
	List *prev;
	List *ret = list;

	for (prev = list; list; list = list->next)
	{
		if (list->data == data)
		{
			prev->next = list->next;
			free (list);
			return ret;
		}

		prev = list;
	}

	return ret;
}

void
list_free (List *list)
{
	List *next;

	while (list)
	{
		next = list->next;
		free (list);
		list = next;
	}
}

void
list_foreach (List *list,
	      ListFunc fn)
{
	for (; list; list = list->next)
	{
		(*fn) (list->data);
	}
}

/* End linked list implementation */

/* Product */

typedef struct
{
	unsigned int id;
	char *name;
	int in_stock;
	double price;
	double discount;
} Product;

Product *
product_new (unsigned int id,
	     const char *name,
	     int in_stock,
	     double price,
	     double discount)
{
	Product *ret;

	ret = malloc (sizeof (Product));

	ret->id = id;
	if (name)
	{
		ret->name = strdup(name);
	}
	else
	{
		ret->name = NULL;
	}
	ret->in_stock = in_stock;
	ret->price = price;
	ret->discount = discount;

	return ret;
}

void
product_free (Product *product)
{
	if (product->name)
	{
		free (product->name);
	}
	free (product);
}

/*
 * Returns a string representation of a Product
 */
char *
product_get_string (Product *product)
{
	char *ret = malloc (sizeof (char) * LINE_BUFFER_SIZE);

	sprintf (ret, "%d,%s,%d,%0.2f,%0.2f", product->id, product->name,
		 product->in_stock, product->price, product->discount);

	return ret;
}

int
product_print_table_header (void)
{
	printf ("+--------+---------------------------------+----------"
		"+-----------+-----------+\n");
	printf ("| %6s | %-31s | %8s | %9s | %9s |\n",
		"ID", "Name", "In stock", "Price", "Disc %");
	printf ("+--------+---------------------------------+----------"
		"+-----------+-----------+\n");
	return 3;
}

int
product_print_table_footer (void)
{
	printf ("+--------+---------------------------------+----------"
		"+-----------+-----------+\n");
	return 1;
}

int
product_print_table_entry (Product *product)
{
	printf ("| %6d | %-31s | %8d | %9.2f | %9.2f |\n",
		product->id, product->name, product->in_stock,
		product->price, product->discount);
	return 1;
}

/* End Product */

/* File save/load ("db_*") */

#define _GET_PRODUCT_MOVE_TO_NEXT_COMMA do { \
	next = strstr (start, ","); \
	if (next == NULL) { \
		printf ("Invalid input line: %s", line); \
		return NULL; \
	} \
} while (0)

static char *
strndup (const char *s, size_t n)
{
	char *r;
	char *t;

	t = r = malloc (sizeof (char) * n);

	for (; n > 0 && *s; t++, s++, n--)
	{
		*t = *s;
	}

	*--t = 0;

	return r;
}

static Product *
get_product_from_line (const char *line)
{
	Product *ret;

	const char *start;
	const char *next;
	/* We'll only strdup() name at the end, so errors don't leak it */
	const char *name_temp;
	size_t name_size;

	unsigned int id;
	char *name;
	int in_stock;
	double price;
	double discount;

	start = line;
	_GET_PRODUCT_MOVE_TO_NEXT_COMMA;

	id = (unsigned int) atoi (start);

	start = next + 1;
	_GET_PRODUCT_MOVE_TO_NEXT_COMMA;

	name_temp = start;
	name_size = (size_t) (next - start + 1);

	start = next + 1;
	_GET_PRODUCT_MOVE_TO_NEXT_COMMA;

	in_stock = atoi (start);

	start = next + 1;
	_GET_PRODUCT_MOVE_TO_NEXT_COMMA;

	price = (double) atof (start);

	start = next + 1;

	discount = (double) atof (start);

	name = strndup (name_temp, name_size);

	ret = product_new (id, name, in_stock, price, discount);

	free (name);

	return ret;
}

List *
db_load_products (void)
{
	FILE *fp;
	char line[LINE_BUFFER_SIZE];
	Product *product;
	List *ret = NULL;

	fp = fopen (PRODUCTS_DB_FILE, "r");
	if (fp == NULL)
	{
		perror ("Error opening " PRODUCTS_DB_FILE);
		return NULL;
	}

	while (fgets (line, LINE_BUFFER_SIZE, fp))
	{
		product = get_product_from_line (line);

		if (product)
		{
			ret = list_prepend (ret, product);
		}
	}

	fclose (fp);

	ret = list_reverse (ret);

	return ret;
}

void
db_write_products (List *list)
{
	FILE *fp;
	char *prod_str;

	fp = fopen (PRODUCTS_DB_FILE, "w");
	if (fp == NULL)
	{
		perror ("Error opening " PRODUCTS_DB_FILE ".new");
		return;
	}

	for (; list; list = list->next)
	{
		prod_str = product_get_string (list->data);
		fputs (prod_str, fp);
		fputc ('\n', fp);
		free (prod_str);
	}

	fclose (fp);
}

int
db_exists (void)
{
	struct stat st;
	int r;

	r = stat (PRODUCTS_DB_FILE, &st);

	return r == 0;
}

void
db_create ()
{
	FILE *fp;

	fp = fopen (PRODUCTS_DB_FILE, "w");
	if (fp == NULL)
	{
		perror ("Error opening " PRODUCTS_DB_FILE ".new");
		return;
	}

	fputs ("0,0,0,0,0\n", fp);

	fclose (fp);
}

int
db_create_with_prompt ()
{
	char c;

	printf ("Create a new inventory file? (Yes, Quit): ");
	fflush (stdout);

	scanf ("%c", &c);

	if (c == 'Y' || c == 'y')
	{
		db_create ();
		return 1;
	}

	if (c != 'Q' && c != 'q')
	{
		printf ("Invalid response\n");
	}

	return 0;
}

/* End Save/Load */

/*
 * Given a product ID, returns the Product (or NULL if not found).
 */
static Product *
product_list_find (List *products,
		   unsigned int id)
{
	Product *product;

	for (; products; products = products->next)
	{
		product = (Product *) products->data;
		if (product->id == id)
		{
			return product;
		}
	}

	return NULL;
}

/* Helpful prompts */

/*
 * Prompts with a question. Returns 1 for affirmative, 0 for negative.
 */
static int
prompt_y_n (const char *prompt)
{
	char input[INPUT_BUFFER_SIZE];
	char *t;
	int valid;

	while (1)
	{
		printf ("%s (y/n)? ", prompt);
		fflush (stdout);

		fgets (input, INPUT_BUFFER_SIZE, stdin);
		
		valid = 1;
		for (t = input + 1; t < input + INPUT_BUFFER_SIZE; t++)
		{
			if (*t == 0) break;
		
			if (*t > 20)
			{
			    valid = 0;
			    break;
			}
		}
		
		if (valid)
		{
			if (*input == 'y' || *input == 'Y') return 1;
			if (*input == 'n' || *input == 'N') return 0;
		}
	
		printf ("Please enter Y or N\n");
	}
	
	/* impossible */
	return 0;
}

/*
 * This is a copy/paste of prompt_y_n () for the one case where asking whether
 * to edit an item is a "e/d" prompt instead of a "y/n" prompt. I find that
 * user interface rather inconsistent, and so I made this function an ugly
 * hack because in the real world it'd disappear early in the program's
 * lifecycle.
 *
 * Returns 1 to edit, 0 if done
 */
static int
prompt_e_d (const char *prompt)
{
	char input[INPUT_BUFFER_SIZE];
	char *t;
	int valid;

	while (1)
	{
		printf ("%s (e/d)? ", prompt);
		fflush (stdout);

		fgets (input, INPUT_BUFFER_SIZE, stdin);
		
		valid = 1;
		for (t = input + 1; t < input + INPUT_BUFFER_SIZE; t++)
		{
			if (*t == 0) break;
		
			if (*t > 20)
			{
			    valid = 0;
			    break;
			}
		}
		
		if (valid)
		{
			if (*input == 'e' || *input == 'E') return 1;
			if (*input == 'd' || *input == 'D') return 0;
		}
	
		printf ("Please enter E or D\n");
	}
	
	/* impossible */
	return 0;
}

/*
 * Prompts for a product ID. Returns the Product.
 */
static Product *
prompt_for_id (List *products,
	       const char *prompt)
{
	char input[INPUT_BUFFER_SIZE];
	Product *product;
	unsigned int id;

	while (1)
	{
		printf ("%s", prompt);
		fflush (stdout);
		fgets (input, INPUT_BUFFER_SIZE, stdin);
		id = (unsigned int) atoi (input);

		product = product_list_find (products, id);

		if (product) return product;

		printf ("Invalid product ID\n");
	}

	/* this will never happen */
	return NULL;
}

/*
 * Pager-type thing: return of 1 means "keep going", 0 means "top".
 */
static int
prompt_continue (void)
{
	char input[INPUT_BUFFER_SIZE];
	char *t;
	int valid;

	while (1)
	{
		printf ("Enter C to continue, Q to stop printing (c/q): ");
		fflush (stdout);

		fgets (input, INPUT_BUFFER_SIZE, stdin);

		valid = 1;
		for (t = input + 1; t < input + INPUT_BUFFER_SIZE; t++)
		{
			if (*t == 0) break;;

			if (*t > 20)
			{
				valid = 0;
				break;
			}
		}

		if (valid)
		{
			switch (*input)
			{
				case 'c':
				case 'C':
				case '\n':
					return 1;
				case 'q':
				case 'Q':
					return 0;
			}
		}

		printf ("Please enter C or Q\n");
	}

	/* Never happens */
	return 0;
}

/*
 * Ask the user to press Enter
 */
static void
prompt_nothing (void)
{
	char input[INPUT_BUFFER_SIZE];

	printf ("Press Enter to continue: ");
	fgets (input, INPUT_BUFFER_SIZE, stdin);
}

/* End helpful prompts */

/*
 * Edits the given item, with prompts.
 *
 * Pass it a NULL product to prompt to create a new item.
 */
static List *
edit_item (List *products,
	   Product *product)
{
	char input[INPUT_BUFFER_SIZE];
	unsigned int id;
	int is_new;
	int want_edit = 0;

	while (product == NULL)
	{
		printf ("Enter an integer product ID: ");
		fflush (stdout);

		fgets (input, INPUT_BUFFER_SIZE, stdin);

		if (*input == 'Q' || *input == 'q')
		{
			return products;
		}

		id = (unsigned int) atoi (input);

		if (product_list_find (products, id) != NULL)
		{
			printf ("Product #%u already exists\n", id);
			printf ("Try another ID or type 'q' to escape\n");
			continue;
		}

		product = product_new (id, NULL, 0, 0, 0);
		products = list_append (products, product);
	}

	is_new = (product->name == NULL);

	want_edit = is_new;
	if (!is_new)
	{
		printf ("Name: %s\n", product->name);
		want_edit = prompt_y_n ("Change name");
	}
	if (want_edit)
	{
		printf ("Enter the product's name: ");
		fflush (stdout);
		fgets (input, INPUT_BUFFER_SIZE, stdin);
		product->name = strndup (input, strlen (input)); /* minus \n */
	}

	want_edit = is_new;
	if (!is_new)
	{
		printf ("In stock: %d\n", product->in_stock);
		want_edit = prompt_y_n ("Change quantity");
	}
	if (want_edit)
	{
		printf ("Enter quantity in stock: ");
		fflush (stdout);
		fgets (input, INPUT_BUFFER_SIZE, stdin);
		product->in_stock = atoi (input);
	}

	want_edit = is_new;
	if (!is_new)
	{
		printf ("Price: %0.2f\n", product->price);
		want_edit = prompt_y_n ("Change price");
	}
	if (want_edit)
	{
		printf ("Enter price: ");
		fflush (stdout);
		fgets (input, INPUT_BUFFER_SIZE, stdin);
		product->price = (double) atof (input);
	}

	want_edit = is_new;
	if (!is_new)
	{
		printf ("Discount: %0.2f%%\n", product->discount);
		want_edit = prompt_y_n ("Change discount");
	}
	if (want_edit)
	{
		printf ("Enter discount in %%: ");
		fflush (stdout);
		fgets (input, INPUT_BUFFER_SIZE, stdin);
		product->discount = (double) atof (input);
	}

	return products;
}

static void
view_product (Product *product)
{
	printf ("ID: %d\n"
		"Name: %s\n"
		"In stock: %d\n"
		"Price: $%0.2f\n"
		"Discount: %0.2f%%\n",
		product->id, product->name, product->in_stock,
		product->price, product->discount);
}

static void
menu_print_main (void)
{
	printf ("\n"
		"ABC Inventory menu\n"
		"-----------------------------\n"
		"1. New Item\n"
		"2. Edit Existing item\n"
		"3. View Existing item\n"
		"4. Delete Existing item\n"
		"5. List all items in inventory\n"
		"Q. Quit Program\n");
}

static List *
menu_create_item (List *products)
{
	return edit_item (products, NULL);
}

static List *
menu_edit_item (List *products)
{
	Product *product;

	if (products == NULL)
	{
		printf ("There are no products to edit\n");
		return products;
	}

	product = prompt_for_id (products,
				 "Enter the ID of the product to edit: ");
	view_product (product);
	if (prompt_e_d ("Edit this product"))
	{
		return edit_item (products, product);
	}
	else
	{
		return products;
	}
}

static void
menu_view_item (List *products)
{
	Product *product;

	if (products == NULL)
	{
		printf ("There are no products to view\n");
		return;
	}

	product = prompt_for_id (products,
				 "Enter the ID of the product to view: ");
	view_product (product);
}

static List *
menu_delete_item (List *products)
{
	Product *product;
	int sure;

	if (products == NULL)
	{
		printf ("There are no products to delete\n");
		return products;
	}

	product = prompt_for_id (products,
				 "Enter the ID of the product to delete: ");
	printf ("Product: %s\n", product->name);

	sure = prompt_y_n ("Are you sure you want to delete this product");
	if (sure)
	{
		products = list_remove (products, product);
		product_free (product);
	}
	else
	{
		printf ("The product was not deleted\n");
	}

	return products;
}

static void
menu_list_items (List *products)
{
	int rows;

	if (products == NULL)
	{
		printf ("There are no products in the database\n");
		return;
	}
	
	rows = NUM_OUTPUT_ROWS;
	rows -= product_print_table_header ();

	for (; products; products = products->next)
	{
		rows -= product_print_table_entry ((Product *) products->data);

		if (rows == 1)
		{
			if (!prompt_continue ())
			{
				return;
			}

			rows = NUM_OUTPUT_ROWS;
		}
	}

	rows -= product_print_table_footer ();

	prompt_nothing ();
}

static void
main_loop (void)
{
	List *products;
	char input[INPUT_BUFFER_SIZE];
	char *t;
	int invalid;

	products = db_load_products ();

	while (1)
	{
		menu_print_main ();
		fgets (input, INPUT_BUFFER_SIZE, stdin);

		/* Check if input is longer than one character */
		invalid = 0;
		for (t = input + 1; t < input + INPUT_BUFFER_SIZE; t++)
		{
			if (*t == 0) break;

			if (*t > 20)
			{
				invalid = 1;
				break;
			}
		}

		if (invalid)
		{
			printf ("Invalid input string\n");
			continue;
		}

		if (*input == 'q' || *input == 'Q')
		{
			break;
		}

		switch (*input)
		{
			case '1':
				products = menu_create_item (products);
				break;
			case '2':
				products = menu_edit_item (products);
				break;
			case '3':
				menu_view_item (products);
				break;
			case '4':
				products = menu_delete_item (products);
				break;
			case '5':
				menu_list_items (products);
				break;
			default:
				printf ("Invalid selection '%c'\n\n", *input);
		}
	}

	db_write_products (products);

	list_foreach (products, LIST_FUNC (product_free));
	list_free (products);
}

static int
main_with_input (const char *cmd,
		 const char *id_str)
{
	unsigned int id;
	List *products;
	Product *product;
	int error = 0;

	products = db_load_products ();

	id = (unsigned int) atoi (id_str);

	product = product_list_find (products, id);
	if (product == NULL && toupper (*cmd) != 'N' && toupper (*cmd) != 'L')
	{
		printf ("Product with ID %d does not exist\n", id);
		error = 1;
	}
	if (product != NULL && toupper (*cmd) == 'N')
	{
		printf ("Product with ID %d already exists\n", id);
		error = 1;
	}

	if (error)
	{
		list_foreach (products, LIST_FUNC (product_free));
		list_free (products);
		return 1;
	}

	switch (toupper (*cmd))
	{
		case 'N':
			product = product_new (id, NULL, 0, 0, 0);
			products = list_append (products, product);
			/* fall through */
		case 'E':
			products = edit_item (products, product);
			db_write_products (products);
			break;
		case 'V':
			view_product (product);
			break;
		case 'D':
			view_product (product);
			if (prompt_y_n ("Delete this item")) {
				products = list_remove (products, product);
				product_free (product);
				db_write_products (products);
			}
			break;
		case 'L':
			menu_list_items (products);
			break;
	}

	list_foreach (products, LIST_FUNC (product_free));
	list_free (products);

	return 0;
}

static void
usage (const char *prog)
{
	printf ("Usage: %s [<option> <id>]\n"
		"  Where <option> is one of:\n"
		"    N - new item\n"
		"    E - edit item\n"
		"    V - view item\n"
		"    D - delete item\n"
		"    L - list all items\n"
		"  <id> is a product ID -- always enter one, even for L\n",
		prog);
}

int main(int argc, char **argv)
{
	if (!db_exists ())
	{
		if (!db_create_with_prompt ())
		{
			exit (0);
		}
	}

	if (argc == 1)
	{
		main_loop ();
		return 0;
	}

	if (argc == 3)
	{
		if (strlen (argv[1]) != 1)
		{
			usage (argv[0]);
			return 1;
		}

		switch (toupper (*argv[1]))
		{
			case 'N':
			case 'E':
			case 'V':
			case 'D':
			case 'L':
				break;
			default:
				usage (argv[0]);
				return 1;
		}

		return main_with_input (argv[1], argv[2]);
	}

	usage (argv[0]);
	return 1;
}




