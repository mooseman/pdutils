/* parser.c : PUBLIC DOMAIN - Jon Mayo - January 12, 2008
 * - You may remove any comments you wish, modify this code any way you wish,
 *   and distribute any way you wish.*/
/* A template for reading in a text file and parsing it. 
 * You can extend it to parse more than just single words, but generally it is
 * best to keep your format as simple as possible. (less work) 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* modifies string at s to make a word, updates e to point after word. */
static void parse_word(char *s, char **e) {
	char *t; /* temp */
	for(t=s;*t && !isspace(*t);t++) ; if(*t) *t++=0;
	*e=t;
}

/* updates s to point to start of next word */
static void next_word(char **s, char *e) {
	while(isspace(*e)) e++;
	*s=e;
}

/* parses an entire line. return a string to report an error */
static const char *parse_line(char *s) {
	char *e;

	fprintf(stderr, "OP =");

	while(*s) {
		parse_word(s, &e);

		if(strcmp(s, "quit")==0) {
			return "entered quit"; /* error */
		}

		fprintf(stderr, " '%s'", s);
		
		next_word(&s, e);
	}

	fprintf(stderr, "\n");
	
	return NULL;
}

static int parse_file(FILE *f, const char *filename) {
	char buf[256];
	char *s;
	unsigned i;
	unsigned line=0;
	const char *reason;
	int ret=1;
	while(line++, fgets(buf, sizeof buf, f)) {
		/* leading spaces */
		for(s=buf;isspace(*s);s++) ;

		/* trim comments */
		for(i=0;s[i];i++) {
			if(s[i]==';') s[i]=0;
		}
	
		if(!*s) continue; /* ignore empty lines */

		reason=parse_line(s);
		if(reason) {
			fprintf(stderr, "%s:%u:parse error:%s\n", filename, line, reason);
			ret=0;
			break;
		}
	}
	return ret;
}

static int process_file(FILE *f, const char *filename) {
	if(parse_file(f, filename)) {
		/* do something with the parsed output */
		return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
	int i;
	if(argc==1) {
		if(!process_file(stdin, "stdin")) return EXIT_FAILURE;
	} else for(i=1;i<argc;i++) {
		FILE *f;
		f=fopen(argv[i], "r");
		if(!f) { perror(argv[i]); return 0; }
		if(!process_file(f, argv[i])) return EXIT_FAILURE;
		fclose(f);
	}
	return 0;
}
