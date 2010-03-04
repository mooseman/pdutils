/*********************************************************************/
/*                                                                   */
/*  This Program Written By Paul Edwards.                            */
/*  Released to the public domain.                                   */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/
/*                                                                   */
/*  grep - search for string in files                                */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "trav.h"

static int insensitive = 0;
static char *search;
static char *filename = NULL;

static int grepfile(char *file, void *ucb);
static void dogrep(FILE *f, char *search);
static char *ins_strstr(char *buf, char *str);
static int ins_memcmp(const void *buf1, const void *buf2, int len);

int main(int argc, char **argv)
{
    int upto;
    
    if (argc < 2)
    {
        printf("usage: grep [-i] search files\n");
        return (0);
    }
    upto = 1;
    if (strcmp(argv[upto], "-i") == 0)
    {
        insensitive = 1;
        upto++;
    }
    if ((argc - upto) <= 1)
    {
        dogrep(stdin, argv[upto]);
    }
    else
    {
        search = argv[upto];
        trav(argv[upto + 1], NULL, grepfile, NULL);
    }
    return (0);
}

static int grepfile(char *file, void *ucb)
{
    FILE *fp;

    (void)ucb;
    filename = file;    
    fp = fopen(file, "r");
    if (fp != NULL)
    {
        dogrep(fp, search);
        fclose(fp);
    }    
    return (1);
}

static void dogrep(FILE *f, char *search)
{
    static char buf[1000];
    int found;

    while (fgets(buf, sizeof buf, f) != NULL)
    {
        found = 0;
        if (!insensitive)
        {
            if (strstr(buf, search) != NULL)
            {
                found = 1;
            }
        }
        else
        {
            if (ins_strstr(buf, search) != NULL)
            {
                found = 1;
            }
        }
        if (found)
        {
            if (filename != NULL)
            {
                printf("%s: ", filename);
            }
            printf("%s", buf);
        }
    }
    return;
}

static char *ins_strstr(char *buf, char *str)
{
    int lenb;
    int lens;
    int x;
    
    lenb = strlen(buf);
    lens = strlen(str);
    for (x = 0; x <= (lenb - lens); x++)
    {
        if (ins_memcmp(buf + x, str, lens) == 0)
        {
            return (buf + x);
        }
    }
    return (NULL);
}

static int ins_memcmp(const void *buf1, const void *buf2, int len)
{
    int x;
    int ch1;
    int ch2;
    const unsigned char *b1 = buf1;
    const unsigned char *b2 = buf2;
    
    for (x = 0; x < len; x++)
    {
        ch1 = toupper(b1[x]);
        ch2 = toupper(b2[x]);
        if (ch1 < ch2)
        {
            return (-1);
        }
        if (ch1 > ch2)
        {
            return (1);
        }
    }
    return (0);
}
