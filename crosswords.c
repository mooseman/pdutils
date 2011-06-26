/* crosswords.c: a program to generate crossword puzzles */

/* David A. Madore <david.madore@ens.fr> - 2006-09-20 - Public Domain */

/* This program first fills the (square) crossword grid with black
   squares in such a way as to form a solution to the "chess queens"
   problem (place one queen on each line so that no two are on the
   same column or diagonal).  Then it tries to fill the rest of the
   grid with words from the dictionary file.  The dictionary file
   should just contain the vocabulary word, one per line, separated by
   \n characters; its name is given by the DICT environment variable
   or, failing that, by the DEFAULT_DICT define below. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Default dictionary name */
#define DEFAULT_DICT "/usr/share/dict/words"

/* Ignore words longer than this. */
#define MAX_WORD_LENGTH 30

unsigned char *dict_buf; /* Dictionary as one large string */
unsigned char *dict_buf_end; /* Pointer to final 0 of it */
long int dict_words_count; /* Number of words in dictionary */
unsigned char **dict_words; /* Array of word pointers */
long int dict_words_len_count[MAX_WORD_LENGTH]; /* Number of words of each length */
unsigned char **dict_words_len[MAX_WORD_LENGTH]; /* Word pointers for each length */

void
read_dict (FILE *dict_file)
     /* Read dictionary file in core. */
{
  off_t size;

  /* Compute size. */
  if ( fseeko (dict_file, 0, SEEK_END) )
    {
      perror ("fseeko");
      exit (EXIT_FAILURE);
    }
  size = ftello (dict_file);
  if ( size == (off_t)-1 )
    {
      perror ("ftello");
      exit (EXIT_FAILURE);
    }
  rewind (dict_file);
  /* Allocate size. */
  dict_buf = malloc (size+1);
  if ( dict_buf == NULL )
    {
      perror ("malloc");
      exit (EXIT_FAILURE);
    }
  /* Read dictionary. */
  size = fread (dict_buf, 1, size, dict_file);
  if ( size < 0 || ferror (dict_file) )
    {
      perror ("fread");
      exit (EXIT_FAILURE);
    }
  dict_buf[size] = 0;
  dict_buf_end = dict_buf+size;
}

void
splice_dict (void)
     /* Splice dictionary at line ends. */
{
  long int wcount;
  unsigned char *ptr;

  /* Do splicing and preliminary word count. */
  wcount = 1;
  for ( ptr = dict_buf ; ptr < dict_buf_end ; ptr++ )
    {
      if ( *ptr == '\n' )
	*ptr = 0;
      if ( *ptr == 0 )
	wcount++;
    }
  /* Allocate word array. */
  dict_words = malloc (sizeof(*dict_words) * wcount);
  if ( dict_words == NULL )
    {
      perror ("malloc");
      exit (EXIT_FAILURE);
    }
  /* Now fill words (skipping over zero-sized words). */
  wcount = 0;
  dict_words[wcount] = dict_buf;
  if ( dict_buf[1] )
    wcount++;
  for ( ptr = dict_buf ; ptr < dict_buf_end ; ptr++ )
    {
      if ( *ptr == 0 )
	{
	  dict_words[wcount] = ptr+1;
	  if ( ptr[1] )
	    wcount++;
	}
    }
  dict_words_count = wcount;
}

void
classify_dict (void)
     /* Classify words by length. */
{
  int ln;
  long int w;

  /* First count words of each length. */
  for ( ln=0 ; ln<MAX_WORD_LENGTH ; ln++ )
    dict_words_len_count[ln] = 0;
  for ( w=0 ; w<dict_words_count ; w++ )
    {
      ln = strlen((char *)dict_words[w]);
      if ( ln < MAX_WORD_LENGTH )
	dict_words_len_count[ln]++;
    }
  /* Now allocate arrays. */
  for ( ln=0 ; ln<MAX_WORD_LENGTH ; ln++ )
    {
      dict_words_len[ln] = malloc (sizeof(*dict_words) * dict_words_len_count[ln]);
      if ( dict_words_len[ln] == NULL )
	{
	  perror ("malloc");
	  exit (EXIT_FAILURE);
	}
    }
  /* Print stats. */
  printf ("Word size stats:\n");
  for ( ln=0 ; ln<MAX_WORD_LENGTH ; ln++ )
    printf ("size %d: %ld words\n", ln, dict_words_len_count[ln]);
  /* Actual classifying */
  for ( ln=0 ; ln<MAX_WORD_LENGTH ; ln++ )
    dict_words_len_count[ln] = 0;
  for ( w=0 ; w<dict_words_count ; w++ )
    {
      ln = strlen ((char *)dict_words[w]);
      if ( ln < MAX_WORD_LENGTH )
	dict_words_len[ln][dict_words_len_count[ln]++] = dict_words[w];
    }
}

/* Size of grid: height should be equal to width for queen placement! */
#define HEIGHT 10
#define WIDTH 10

/* Number of characters in alphabet (shouldn't be changed) */
#define NBALPH 256

/* A constraint vector: .a[] indicates whether each char is possible,
   and nba stores number of 1's in this vector. */
typedef struct allowed_s {
  char a[NBALPH];
  int nba;
} allowed_t;

void
clear_allowed (allowed_t *all)
     /* Clear a constraint vector (disallow everything). */
{
  int k;

  for ( k=0 ; k<NBALPH ; k++ )
    all->a[k] = 0;
  all->nba = 0;
}

void
fill_allowed (allowed_t *all)
     /* Fill a constraint vector (allow everything). */
{
  int k;

  for ( k=0 ; k<NBALPH ; k++ )
    all->a[k] = 1;
  all->nba = NBALPH;
}

char
add_to_allowed (allowed_t *all, int k)
     /* Allow k in a constraint vector. */
{
  char former = all->a[k];
  all->a[k] = 1;
  all->nba += !former;
  return former;
}

char
remv_from_allowed (allowed_t *all, int k)
     /* Disallow k in a constraint vector. */
{
  char former = all->a[k];
  all->a[k] = 0;
  all->nba -= former;
  return former;
}

/* A square of a grid: */
typedef struct square_s {
  char black; /* Is this a black square? */
  char known; /* Is this fixed? */
  unsigned char value; /* If so, what is its value? */
  allowed_t allowed; /* Constraint vector for this square. */
} square_t;

/* A grid */
typedef square_t board_t[HEIGHT][WIDTH];

void
init_board_allowed (board_t board)
     /* Initialize all constraint vectors in a board. */
{
  int i, j;

  for ( i=0 ; i<HEIGHT ; i++ )
    for ( j=0 ; j<WIDTH ; j++ )
      {
	if ( board[i][j].black )
	  {
	    board[i][j].known = 1;
	    board[i][j].value = 0;
	  }
	if ( board[i][j].known )
	  {
	    clear_allowed (&board[i][j].allowed);
	    add_to_allowed (&board[i][j].allowed, board[i][j].value);
	  }
	else
	  fill_allowed (&board[i][j].allowed);
      }
}

char
search_horiz (board_t board, int line, int cbegin, int cend)
     /* Restrict constraints on the board by placing horizontal words
	on line line between columns cbegin and cend-1: try each word
	in the dictionary and compute new constraints accordingly;
	return 1 if the constraints have been effectively reduced. */
{
  long int w;
  int k;
  allowed_t blurb[WIDTH];
  char changed;
  long int nbok;

  for ( k=0 ; k<cend-cbegin ; k++ )
    clear_allowed (&blurb[k]);
  nbok = 0;
  /* Loop over all words of correct length.x */
  for ( w=0 ; w<dict_words_len_count[cend-cbegin] ; w++ )
    {
      char ok = 1;
      /* Check whether it fits. */
      for ( k=0 ; k<cend-cbegin ; k++ )
	if ( !dict_words_len[cend-cbegin][w][k]
	     || !board[line][cbegin+k].allowed.a[(int)dict_words_len[cend-cbegin][w][k]] )
	  {
	    ok = 0;
	    break;
	  }
      /* Useless size check */
      if ( ok && dict_words_len[cend-cbegin][w][cend-cbegin] )
	ok = 0;
      if ( ok )
	{
	  /* We found a word: enlarge allowed list */
	  for ( k=0 ; k<cend-cbegin ; k++ )
	    add_to_allowed (&blurb[k], dict_words_len[cend-cbegin][w][k]);
	  nbok++;
	}
    }
  /* Now write new constraints. */
  changed = 0;
  for ( k=0 ; k<cend-cbegin ; k++ )
    {
      if ( blurb[k].nba < board[line][cbegin+k].allowed.nba )
	{
	  board[line][cbegin+k].allowed = blurb[k];
	  changed = 1;
	}
    }
  return changed;
}

char
search_vert (board_t board, int lbegin, int lend, int column)
     /* Restrict constraints on the board by placing verctical words
	on column column between lines lbegin and lend-1: try each
	word in the dictionary and compute new constraints
	accordingly; return 1 if the constraints have been effectively
	reduced. */
{
  long int w;
  int k;
  allowed_t blurb[HEIGHT];
  char changed;
  long int nbok;

  for ( k=0 ; k<lend-lbegin ; k++ )
    clear_allowed (&blurb[k]);
  nbok = 0;
  /* Loop over all words of correct length.x */
  for ( w=0 ; w<dict_words_len_count[lend-lbegin] ; w++ )
    {
      char ok = 1;
      /* Check whether it fits. */
      for ( k=0 ; k<lend-lbegin ; k++ )
	if ( !dict_words_len[lend-lbegin][w][k]
	     || !board[lbegin+k][column].allowed.a[(int)dict_words_len[lend-lbegin][w][k]] )
	  {
	    ok = 0;
	    break;
	  }
      /* Useless size check */
      if ( ok && dict_words_len[lend-lbegin][w][lend-lbegin] )
	ok = 0;
      if ( ok )
	{
	  /* We found a word: enlarge allowed list */
	  for ( k=0 ; k<lend-lbegin ; k++ )
	    add_to_allowed (&blurb[k], dict_words_len[lend-lbegin][w][k]);
	  nbok++;
	}
    }
  /* Now write new constraints. */
  changed = 0;
  for ( k=0 ; k<lend-lbegin ; k++ )
    {
      if ( blurb[k].nba < board[lbegin+k][column].allowed.nba )
	{
	  board[lbegin+k][column].allowed = blurb[k];
	  changed = 1;
	}
    }
  return changed;
}

void
compute_board_allowed (board_t board)
     /* Compute constraints on a board: iteratively place horizontal
	and vertical words until constraints remain unchanged. */
{
  char changed;
  int i, j, wbegin, wend;

  do
    {
      changed = 0;
      /* Try all horizontal words. */
      for ( i=0 ; i<HEIGHT ; i++ )
	{
	  wbegin = 0;
	  for ( j=0 ; j<=WIDTH ; j++ )
	    if ( j==WIDTH || board[i][j].black )
	      {
		wend = j;
		if ( wend > wbegin )
		  {
		    if ( search_horiz (board, i, wbegin, wend) )
		      changed = 1;
		  }
		wbegin = j+1;
	      }
	}
      /* Try all vertical words. */
      for ( j=0 ; j<WIDTH ; j++ )
	{
	  wbegin = 0;
	  for ( i=0 ; i<=HEIGHT ; i++ )
	    if ( i==HEIGHT || board[i][j].black )
	      {
		wend = i;
		if ( wend > wbegin )
		  {
		    if ( search_vert (board, wbegin, wend, j) )
		      changed = 1;
		  }
		wbegin = i+1;
	      }
	}
    }
  while ( changed );
}

char
find_recurse (board_t board, int *line, int *column)
     /* Examine constraints of various squares to find the one to
	recurse on.  Return 1 if there is further recursion to be done
	(board is not complete). */
{
  int i, j;
  int besti, bestj, bestv;
  char yes;

  yes = 0;
  bestv = NBALPH+1;
  for ( i=0 ; i<HEIGHT ; i++ )
    for ( j=0 ; j<WIDTH ; j++ )
      {
	if ( board[i][j].allowed.nba == 1 )
	  {
	    /* Square has but one allowed value: consider it fixed
	       (this is just an optimization).  Note that if the
	       square has _zero_ allowed values, we will return it:
	       the caller will understand that the board is impossible
	       and bail out (backtrack). */
	    int c;
	    board[i][j].known = 1;
	    for ( c=0 ; c<NBALPH ; c++ )
	      if ( board[i][j].allowed.a[c] )
		{
		  board[i][j].value = c;
		  break;
		}
	  }
	if ( ! board[i][j].known && board[i][j].allowed.nba < bestv )
	  {
	    /* Most constrained so far. */
	    yes = 1;
	    bestv = board[i][j].allowed.nba;
	    besti = i;
	    bestj = j;
	  }
      }
  if ( yes )
    {
      *line = besti;
      *column = bestj;
    }
  return yes;
}

void
print_board (board_t board)
     /* User-friendly board display */
{
  int i, j;

  for ( i=0 ; i<HEIGHT ; i++ )
    {
      for ( j=0 ; j<WIDTH ; j++ )
	{
	  if ( board[i][j].black )
	    printf (" ");
	  else if ( ! board[i][j].known )
	    printf ("?");
	  else
	    printf ("%c", board[i][j].value);
	}
      printf ("\n");
    }
}

void
finalize (board_t board)
     /* Called when a solution is found. */
{
  printf ("Found solution:\n");
  print_board (board);
  exit (EXIT_SUCCESS);
}

void
board_recurse (int level, board_t board)
     /* Main recursive function: try to find a solution from a given
	board of black and fixed squares (note that constraint vectors
	must have been correctly computed: if in doubt, use
	init_board_allowed()). */
{
  board_t newboard;
  int i, j;

  printf ("Level %d:\n", level);
  print_board (board);
  compute_board_allowed (board);
  if ( find_recurse (board, &i, &j) )
    {
      int k, l;
      square_t sq;
      char tried[NBALPH];

      sq = board[i][j];
      if ( ! sq.allowed.nba )
	{
	  /* Impossible constraints: bail out (backtrack)! */
	  printf ("Need to backtrack from here (no solutions in (%d,%d)):\n",
		  i, j);
	  print_board (board);
	  return;
	}
      /* Now try fixing the (i,j) square to all possible values, in a
	 random order. */
      for ( k=0 ; k<NBALPH ; k++ )
	tried[k] = 0;
      for ( l=0 ; l<NBALPH ; l++ )
	{
	  do
	    k = rand()%NBALPH;
	  while ( tried[k] );
	  tried[k] = 1;
	  if ( sq.allowed.a[k] )
	    {
	      /* Copy board on the stack (as we need to modify it) */
	      memcpy (newboard, board, sizeof newboard);
	      /* Recurse on this value. */
	      newboard[i][j].known = 1;
	      newboard[i][j].value = k;
	      clear_allowed (&newboard[i][j].allowed);
	      add_to_allowed (&newboard[i][j].allowed, k);
	      board_recurse (level+1, newboard);
	    }
	}
    }
  else
    finalize (board);
}

/* Solution of the queens' problem (queens[i] is the column of the
   queen on line i) */
int queens[HEIGHT];

char
queens_recurse (int level)
     /* Solve the queens' problem recursively: return 1 when a
	solution is found. */
{
  int k, v;
  int i;
  char tried[WIDTH];

  if ( level >= HEIGHT )
    return 1;
  /* Try all columns in a random order. */
  for ( v=0 ; v<WIDTH ; v++ )
    tried[v] = 0;
  for ( k=0 ; k<WIDTH ; k++ )
    {
      char ok = 1;
      do
	v = rand()%WIDTH;
      while ( tried[v] );
      tried[v] = 1;
      for ( i=0 ; i<level ; i++ )
	if ( v == queens[i] || abs(v-queens[i])==abs(level-i) )
	  ok = 0;
      if ( ok )
	{
	  /* We can place one further queen, satisfying all
	     constraints so far: recurse one level deeper. */
	  queens[level] = v;
	  if ( queens_recurse (level+1) )
	    return 1; /* Return as soon as a solution is found. */
	}
    }
  return 0;
}

void
init_board (board_t board)
     /* Initial placement of black squares. */
{
  int i, j;

  /* Compute a solution of the queens' problem. */
  if ( ! queens_recurse (0) )
    {
      printf ("Cannot place queens!\n");
      exit (EXIT_FAILURE);
    }
  /* Place black squares accordingly. */
  for ( i=0 ; i<HEIGHT ; i++ )
    for ( j=0 ; j<WIDTH ; j++ )
      {
	board[i][j].black = queens[i]==j;
	board[i][j].known = board[i][j].black;
      }
  /* Initiate constraints accordingly. */
  init_board_allowed (board);
}

int
main (void)
     /* Main program */
{
  const char *dict_name;
  FILE *dict_file;
  board_t board;

  /* Initialize random number generator. */
  srandom (time (NULL));
  /* Read dictionary file */
  dict_name = getenv ("DICT");
  if ( ! dict_name )
    dict_name = DEFAULT_DICT;
  dict_file = fopen (dict_name, "r");
  if ( ! dict_file )
    {
      perror ("fopen");
      exit (EXIT_FAILURE);
    }
  read_dict (dict_file);
  splice_dict ();
  classify_dict ();
  /* Initialize board. */
  init_board (board);
  /* Now do it! */
  board_recurse (0, board);
  /* If we finished recursion and found no solution (otherwise
     finalize() would have been called), then none exists. */
  printf ("No solution exists!\n");
  exit (EXIT_FAILURE);
}
