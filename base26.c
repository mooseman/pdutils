/* base26.c : deals with alphabetic counting typically seen in spreadsheets */
/* PUBLIC DOMAIN 2005 - Jon Mayo
 * - You may remove any comments you wish, modify this code any way you wish,
 *   and distribute any way you wish. */

/* This is a bijective base-26 numeration system (aka 26-adic). Just like the
 * column labels in popular spreadsheet programs. I have called it base-26, but
 * strictly speaking that is not the precise name. At the time I wrote it, I
 * did not know what it was called. Sometimes it is called hexavigesimal, but
 * that just means base-26. 
 *
 * it goes A..Z, AA..AZ, BA..BZ, ...
 * This program can convert to the system and from the system. (using -e option)
 * 'A' starts at 0, 'Z' is 25, 'AA' is 26, etc. We use -1 as empty string.
 *
 * when using signed 64-bit value the largest is 9223372036854775807 which
 * encodes as crpxnlskvljfhh.
 *
 * usage:
 * base26 -e 675 25 26 0 1 474551
 *
 * enjoy! 
 */
/* further information:
 * http://en.wikipedia.org/wiki/Bijective_numeration
 * http://en.wikipedia.org/wiki/Hexavigesimal
 */
/* 2006.12.18 - JDM
 * fixed stack overflow, implementation bugs and supports empty string as -1,
 * enhanced test routine.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const char tab26[26] = "abcdefghijklmnopqrstuvwxyz";

/* this code uses C99, change if you do not have long long type */
typedef long long base26_num_t;

/* this is returned for an empty string */
#define BASE26_EMTPY -1

int base26encode(base26_num_t n, size_t len, char *str)
{
    /* TODO: test how well 'len' prevents overflows */
    char buf[16];
    unsigned i;
    if(n<0) { /* empty */
        *str=0;
        return 0;
    }
    n++;
    i=0;
    if(len>(sizeof buf+1)) {
        len=sizeof buf+1; /* buf doesn't need a null terminator but the buffer does */
    }
    while(i<(len-1)) {
        n--;
        buf[sizeof buf-i-1]=tab26[n%26];
        i++;
        n/=26;
        if(n<=0) break;
    }
    memcpy(str,buf+sizeof buf-i,i+1);
    str[i]=0;
    return i;
}

base26_num_t base26decode(const char *str)
{
    int i=0;
    base26_num_t n, v;
   
    for(n=0;str[i];i++) {
        /* TODO: use an abstraction to support non-ASCII systems */
        v=tolower(str[i])-'a'+1;
        n=n*26+v;
    }

    return n-1;
}

/* define for testing */
#ifdef STAND_ALONE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NR(x) (sizeof(x)/sizeof*(x))

static int isalphastr(const char *s) {
	while(*s) {
		if(!isalpha(*s)) return 0;
		s++;
	}
	return 1;
}

int main(int argc, char **argv)
{
	if(argc>1) {
		if(!strcmp(argv[1],"-t")) { /* test routine */
			char t[60];
			unsigned i;
			base26_num_t testlist[] = { 0, 1, 2, 24, 26, 27, 28, 676, 677, 702, 17576,
			456976L, 2049655024L, 2, 252, 13706, 96539273536LL, 4437, 342 };
			for(i=0;i<NR(testlist);i++) {
				base26_num_t n;
				base26encode(testlist[i],sizeof t, t);
				n=base26decode(t);
				printf("%s. %lld = %lld\n",t,testlist[i],n);
			}

			for(i=0;i<10000;i++) {
				base26_num_t n;
				base26encode(i,sizeof t, t);
				n=base26decode(t);
                                if(i!=n) {
                                    printf("failed: %s. %u = %lld\n",t,i,n);
                                    return EXIT_FAILURE;
                                }
                        }
                        printf("passed: 0 to %u\n", i-1);
		} else if(!strcmp(argv[1],"-e")) { /* encode numbers as strings */
			int i;
			for(i=2;i<argc;i++) {
				base26_num_t n;
				char t[60];
				n=strtoull(argv[i], 0, 0);
				base26encode(n, sizeof t, t);
				printf("%s ",t);
			}
			printf("\n");
		} else { /* default is to just decode all the strings to numbers */
			int i;
			for(i=1;i<argc;i++) {
				base26_num_t n;
				/* ignore strings that have funny characters in them */
				if(!isalphastr(argv[i])) continue;
				n=base26decode(argv[i]);
				printf("%llu ",n);
			}
			printf("\n");
		}
	}

    return 0;
}
#endif
