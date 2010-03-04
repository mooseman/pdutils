/* lc2.c
   C source file statistics version 2
   count number of code and comment lines
   written by thanh nhan tran 1992
   released to public domain july 1993
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

enum states { EXE, DOC };
struct flags { unsigned exe, doc; };
struct nlines { unsigned long all, blanks, exes, docs, inlines; };

void put_stats(const char *heading, const struct nlines *stats)
{
  printf("statistics of %s\n", heading);
  printf("%20s: %7lu\n", "total", stats->all);
  printf("%20s: %7lu\n", "blanks", stats->blanks);
  printf("%20s: %7lu\n", "codes", stats->exes);
  printf("%20s: %7lu\n", "comments only", stats->docs);
  printf("%20s: %7lu\n", "in-line comments", stats->inlines);
  printf("%20s: %9.1lf\n", "% of codes",
    (double)stats->exes / stats->all * 100.0);
  printf("%20s: %9.1lf\n", "% of comments",
    (double)(stats->docs + stats->inlines) / stats->all * 100.0);
}

void add_total(struct nlines *total, const struct nlines *file)
{
  total->all += file->all;
  total->blanks += file->blanks;
  total->exes += file->exes;
  total->docs += file->docs;
  total->inlines += file->inlines;
}

void add_count(struct nlines *stats, const struct flags *status)
{
  stats->all++;
  if (status->exe) {
    stats->exes++;
    if (status->doc) stats->inlines++;
  }
  else if (status->doc)
    stats->docs++;
  else stats->blanks++;
}

int parse(const char *text, struct nlines *stats)
{
  static enum states state = EXE;
  static int isinstr = 0;
  struct flags status;

  for (memset(&status, 0, sizeof(status)); *text; text++)
    if (!isspace(*text))
      if (state == DOC) {
        status.doc++;
        if (*text == '*' && *(text + 1) == '/')
          text++, state = EXE;
      }
      else { /* state == EXE */
        if (isinstr) {
          if (*text == '\\')
            text++;
          else if (*text == '\"')
            isinstr = !isinstr;
        }
        else if (*text == '/') {
          if (*(text + 1) == '*') {
            text--, state = DOC;
            continue;
          }
          else if (*(text + 1) == '/') {
            status.doc++;
            break;
          }
        }
        status.exe++;
      }
  add_count(stats, &status);
  return (0);
}

int count(const char *fname, struct nlines *total)
{
  struct nlines stats;
  FILE *stream = fopen(fname, "r");
  char line[256];

  if (!stream)
    return (printf("error opening %s\n", fname), 1);
  memset(&stats, 0, sizeof(stats));
  while (fgets(line, sizeof(line), stream))
    parse(line, &stats);
  if (!feof(stream))
    return (printf("error reading %s\n", fname), 2);
  if (fclose(stream))
    return (printf("error closing %s\n", fname), 3);
  add_total(total, &stats);
  put_stats(fname, &stats);
  return (0);
}

int arg_ok(int nargs, const char *prog)
{
  if (nargs > 1)
    return (1);
  puts("line counting program for C source files");
  printf("usage: %s fname [fname ...]\n", prog);
  puts("where fname is the name of the file to be counted");
  return (0);
}

int main(int argc, char **argv)
{
  struct nlines total;

  if (!arg_ok(argc, *argv))
    return (1);
  memset(&total, 0, sizeof(total));
  while (*++argv)
    if (count(*argv, &total))
      return (2);
  printf("\nprocessed %d file%c\n", argc - 1, argc > 2 ? 's' : ' ');
  put_stats("all files", &total);
  return (0);
}
