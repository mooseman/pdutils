/* Copyright Abandoned 2000 Monty Program KB & Detron HB
   This file is public domain and comes with NO WARRANTY of any kind */

/* readline for batch mode */

typedef struct st_line_buffer
{
  File file;
  char *buffer;			/* The buffer itself, grown as needed. */
  char *end;			/* Pointer at buffer end */
  char *start_of_line,*end_of_line;
  uint bufread;			/* Number of bytes to get with each read(). */
  uint eof;
  ulong max_size;
} LINE_BUFFER;

extern LINE_BUFFER *batch_readline_init(ulong max_size,FILE *file);
extern LINE_BUFFER *batch_readline_command(my_string str);
extern char *batch_readline(LINE_BUFFER *buffer);
extern void batch_readline_end(LINE_BUFFER *buffer);
