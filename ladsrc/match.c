/* match.c  Gathering of Sec. 21.2 code snippets (thanks to Robert Lynch). */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void do_regerror(int errcode, const regex_t *preg);

int main() {

    regex_t p;
    regmatch_t *pmatch;
    int rcomp_err, rexec_err;
    char string[BUFSIZ+1];
    int i;

    rcomp_err = regcomp(&p, "(^(.*[^\\])#.*$)|(^[^#]+$)",
                REG_EXTENDED | REG_NEWLINE);
    if(rcomp_err) {
        do_regerror(rcomp_err, &p);
    }

    pmatch = alloca(sizeof(regmatch_t) * (p.re_nsub+1));
    if(!pmatch) {
        perror("alloca");
    }

    printf("Input a string: ");
    fgets(string, sizeof(string), stdin);

    rexec_err = regexec(&p, string, p.re_nsub+1, pmatch, 0);
    if(rexec_err) {
        do_regerror(rexec_err, &p);
    } else {
        /* match succeeded */
        for(i = 0; i <= p.re_nsub; i++) {
            /* print the matching portion(s) of the string */
            if(pmatch[i].rm_so != -1) {
                char * submatch;
                size_t matchlen = pmatch[i].rm_eo - pmatch[i].rm_so;
                submatch = malloc(matchlen+1);
                strncpy(submatch, string+pmatch[i].rm_so, matchlen);
		submatch[matchlen] = '\0';
                printf("match %i: %s\n", i, submatch);
                free(submatch);
            }
        }
    }
    exit(0);
}

void do_regerror(int errcode, const regex_t *preg) {
    char *errbuf;
    size_t errbuf_size;

    errbuf_size = regerror(errcode, preg, NULL, 0); /* missing ; */
    errbuf = alloca(errbuf_size);
    if(!errbuf) {
        perror("alloca");
        return;
    }

    regerror(errcode, preg, errbuf, errbuf_size);
    fprintf(stderr, "%s\n", errbuf);
}
