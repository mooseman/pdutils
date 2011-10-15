/* texteditor.c : text editor routines */
/* PUBLIC DOMAIN - J.Mayo - August 12, 2008 */
/* See Also:
 * http://en.wikipedia.org/wiki/Gap_buffer
 */
/* BUGS:
 */
/* TESTCASES:
 */
/* FIXED:
 * 080813 - the tests are showing the end of the buffer is getting cut off.
 *        : was not incrementing seq->len in the insert routine
 */
/* TODO:
 * + move() function (must use textseq_expand_gap() support)
 * + substr() function to copy part of the sequence into a string
 * + linelen() function to find the length of a line at the cursor
 * + strstr() can search each half, but when it fails on the first manually check the boundry.
 * + test out strchr() and others on sequence
 * + support regexec() on sequence (must consolidate seq first)
 * + insert, delete, move take unsigned *cursors, unsigned nr_cursors to update
 * + rename everything texteditor_xxxx() or something
 * + document texteditor API
 */
/* DONE:
 * + keep null terminator at end of buffer and at start of gap. then strxxx ops will work
 * + realloc() when gap is not large enough on insert
 * + if buffer does not have a newline at the end, add one before appending
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG(fmt, ...) fprintf(stderr, "%s:%d:" fmt, __FILE__, __LINE__, ## __VA_ARGS__);
#define TRACE_EXPR(e) fprintf(stderr, "EXPR:" #e " is %zu\n", (size_t)(e));

/**************************************************************************/
/* Sequences */

#define ROUNDUP(len, size) ((len)+(size)-1)/(size)*(size)
#define MINIMUM_GAP_SIZE 200	/* used on loading a fresh buffer or reallocing */

struct textseq {
	char *data;
	size_t len; /* current length - not including gap */
	size_t max; /* limit on maximum length */
	size_t alloc; /* amount allocated for string */
	size_t gap_pos, gap_len; /* buffer-gap algorithm */
};

#if 0 /* UNUSED */
/* turns a virtual position into a raw buffer offset */
inline unsigned textseq_raw_offset(struct textseq *seq, unsigned position) {
	assert(seq!=NULL);
	DEBUG("pos:%zd len:%zd\n", position, seq->len);
	assert(position<=seq->len);
	if(position>seq->gap_pos) {
		return position-seq->gap_pos;
	} else {
		return position;
	}
}
#endif

int textseq_init(struct textseq *seq, const char *str) {
	assert(seq!=NULL);
	seq->len=strlen(str);
	seq->alloc=ROUNDUP(seq->len+MINIMUM_GAP_SIZE, 4096); /* round up 4K */
	/* tests for difficult scenarios: */
	// seq->alloc=seq->len+20;
	// seq->alloc=seq->len+1;
	// seq->alloc=seq->len;
	seq->max=seq->alloc*16; /* max defaults to 16 times the initial size */
	seq->data=malloc(seq->alloc);
	seq->data[seq->alloc-1]=0; /* null terminate the buffer */
	DEBUG("alloc: %zu\n", seq->alloc);
	if(!seq->data) {
		perror("malloc()");
		return 0;
	}
	seq->gap_pos=seq->len; /* end of buffer */
	seq->gap_len=seq->alloc-seq->len-1; /* leave room for null terminator */
	memcpy(seq->data, str, seq->len+1); /* include null terminator for the gap */
	return 1;
}

void textseq_free(struct textseq *seq) {
	if(!seq)
		return;
	free(seq->data);
	seq->data=NULL;
}

/* returns a cursor at EOF */
inline int textseq_cursor_eof(struct textseq *seq) {
	assert(seq!=NULL);
	return seq->len;
}

/* returns a VIRTUAL cursor at a character based offset */
inline int textseq_cursor_charat(struct textseq *seq, unsigned offset) {
	assert(seq!=NULL);

	return offset;
}

/* returns a VIRTUAL cursor at EOF */
inline int textseq_cursor_charateof(struct textseq *seq) {
	assert(seq!=NULL);

	return seq->len;
}

/* returns a cursor to a line at the position of the line's start,
 * position is the VIRTUAL position in the buffer, the gap taken into account.
 */
int textseq_cursor_lineat(struct textseq *seq, unsigned line) {
	size_t i;
	unsigned curr_line;

	assert(seq!=NULL);
	assert(line!=(unsigned)-1); /* this used to have special meaning */

	/* first line is a special case */
	if(line==0)
		return 0;

	curr_line=0;
	/* walk first half of buffer-gap */
	DEBUG("gap=%zu gaplen=%zu len=%zu\n", seq->gap_pos, seq->gap_len, seq->len);
	assert(seq->gap_pos<=(seq->len+seq->gap_len));
	for(i=0;i<seq->gap_pos;i++) {
		if(seq->data[i]=='\n') {
			curr_line++;
			if(curr_line==line) {
				return i+1; /* found the line */
			}
		}
	}

#ifdef __OPTIMIZE__
	if(i>=seq->len)
#else
	if(i+seq->gap_len>=seq->len+seq->gap_len)
#endif
	{
		DEBUG("Line does not exist\n");
		return -1; /* line doesn't exist - fallen off the end */
	}

	for(;i<seq->len;i++) {
		if(seq->data[i+seq->gap_len]=='\n') {
			curr_line++;
			if(curr_line==line) {
				return i+1; /* found the line */
			}
		}
	}

	DEBUG("Line does not exist\n");
	return -1; /* line doesn't exist - fallen off the end */
}

/* cursor must be at start of line */
int textseq_linelength(struct textseq *seq, unsigned cursor) {
	size_t i, len;
	if(cursor>seq->len)
		return -1; /* failure - line does not exist */

	len=0;
	i=cursor;
	/* first half */
	for(;i<seq->gap_pos;i++,len++) {
		if(seq->data[i]=='\n') {
			return len;
		}
	}
	/* last half */
	i+=seq->gap_len;
	for(;i<seq->len+seq->gap_len;i++,len++) {
		if(seq->data[i]=='\n') {
			return len;
		}
	}
	return len; /* no newline at end of buffer */
}

void textseq_dump(struct textseq *seq, int tag) {
	size_t i;
	printf("=== dump #%d ===  total length: %zu (gap:%zu len:%zu)\n", tag, seq->len+seq->gap_len, seq->gap_pos, seq->len);
	for(i=0;i<seq->gap_pos;i++) {
		putchar(seq->data[i]);
	}
	if(seq->gap_len) {
		printf("<<GAP:%zd>>", seq->gap_len);
	}
	for(i=seq->gap_pos+seq->gap_len;i<seq->len+seq->gap_len;i++) {
		putchar(seq->data[i]);
	}

	printf("<EOF>\n");
}

/* grows the buffer gap by at least minimum_size */
int textseq_expand_gap(struct textseq *seq, unsigned minimum_size) {
	char *tmp;
	size_t newsize, taillen, newgaplen;
	minimum_size+=MINIMUM_GAP_SIZE;
	if(seq->gap_len>minimum_size)
		return 1; /* already large enough */

	newsize=ROUNDUP(seq->len+minimum_size, 4096); /* round up 4K */

	DEBUG("realloc: data=%p newsize=%zu alloc=%zu\n", seq->data, newsize, seq->alloc);
	tmp=realloc(seq->data, newsize);
	assert(tmp!=NULL);
	if(!tmp) {
		perror("realloc()");
		return 0; /* failure */
	}

#ifndef NDEBUG
	/* fill the newly created region with junk data */
	memset(&tmp[seq->alloc], '#', newsize-seq->alloc);
#endif

	/* move tail data to end */
	taillen=seq->len-seq->gap_pos; /* gap_pos is the head size */
	newgaplen=newsize-seq->len-1;
	DEBUG("memmove: data=%p gap_post=%zu gap_len=%zu len=%zu newgaplen=%zu taillen=%zu\n", tmp, seq->gap_pos, seq->gap_len, seq->len, newgaplen, taillen);
	TRACE_EXPR(seq->gap_pos+newgaplen+taillen);
	assert((seq->gap_pos+newgaplen+taillen)<=newsize);
	memmove(&tmp[seq->gap_pos+newgaplen], &tmp[seq->gap_pos+seq->gap_len], taillen);

	/* commit new values to structure */
	seq->data=tmp;
	seq->alloc=newsize;
	seq->gap_len=newgaplen; /* leave room for null terminator on end */
	seq->data[seq->alloc-1]=0; /* null terminate the buffer */

#ifndef NDEBUG
	/* fill the gap with junk data */
	memset(&seq->data[seq->gap_pos], '@', seq->gap_len);
#endif
	seq->data[seq->gap_pos]=0; /* null terminate the start of the gap */

	{
		size_t i;
		int dumb1=0;
		for(i=0;i<seq->alloc;i++) {
			if(seq->data[i]>'Z')
				dumb1++;
		}
	}

	return 1;
}

/* bounds checking */
#define GAP_BOUNDS_CHECK(seq, src, dst, len) do { \
		DEBUG("data:%p datalen:%#zx gaplen:%#zx src:%p dst:%p len:%#zx\n", seq->data, seq->len, seq->gap_len, src, dst, len); \
		assert(src>=seq->data); \
		assert(src<(seq->data+seq->len+seq->gap_len)); \
		assert(src+len<=(seq->data+seq->len+seq->gap_len)); \
		assert(dst>=seq->data); \
		assert(dst<(seq->data+seq->len+seq->gap_len)); \
		assert(dst+len<=(seq->data+seq->len+seq->gap_len)); } while(0)

/* move gap to a new cursor position */
/* ORIGINAL    UP        DOWN      UP2      DOWN2
 * +-----+   +-----+   +-----+   +-----+   +-----+
 * |     |   |     |   |     |   |     |   |     |
 * |     |   +-----+   |     |   |     |   |     |
 * |     |   | GAP |   |     |   |     |   |     |
 * |     |   +-----+   |     |   +-----+   |     |
 * +-----+   |     |   |     |   | GAP |   |     |
 * | GAP |===|     |===|     |===+-----+===+-----+
 * +-----+   |     |   |     |   |     |   | GAP |
 * |     |   |     |   |     |   |     |   +-----+
 * |     |   |     |   +-----+   |     |   |     |
 * |     |   |     |   | GAP |   |     |   |     |
 * |     |   |     |   +-----+   |     |   |     |
 * |     |   |     |   |     |   |     |   |     |
 * +-----+   +-----+   +-----+   +-----+   +-----+
 */
int textseq_move_gap(struct textseq *seq, unsigned position) {
	char *src, *dst;
	size_t len;

	assert(seq!=NULL);
	assert(position<=(seq->len+seq->gap_len));

	DEBUG("gappos=%zu gaplen=%zu len=%zu\n", seq->gap_pos, seq->gap_len, seq->len);

	/* fast path for when the gap does not need to be moved */
	if(position==seq->gap_pos || seq->gap_len==0) {
		DEBUG("FAST PATH\n");
		return 1;
	}

	if(position>(seq->len+seq->gap_len)) {
		DEBUG("FAILURE\n");
		return 0; /* failure */
	}

	DEBUG("gappos:%zd gaplen:%zd position:%d\n", seq->gap_pos, seq->gap_len, position);

	if(position<=seq->gap_pos) {
		/* UP: we need the move the gap towards the start of the buffer */
		DEBUG("UP\n");
		src=&seq->data[position];
		dst=&seq->data[position+seq->gap_len];
		len=seq->gap_pos-position;
		GAP_BOUNDS_CHECK(seq, src, dst, len);
	} else {
		/* DOWN: we need to move the gap towards the end of the buffer */
		DEBUG("DOWN\n");
		src=&seq->data[seq->gap_pos+seq->gap_len];
		dst=&seq->data[seq->gap_pos];
		len=position-seq->gap_pos;
		GAP_BOUNDS_CHECK(seq, src, dst, len);
	}

	seq->gap_pos=position;
	assert(seq->gap_pos<seq->alloc);
	assert((seq->gap_pos+seq->gap_len-1)<=seq->alloc); /* do not overlap null */
	assert(dst!=src); /* this would have been caught by the fast path above */
	memmove(dst, src, len);

#ifndef NDEBUG
	/* fill the gap with junk data */
	memset(&seq->data[seq->gap_pos], '&', seq->gap_len);
#endif
	seq->data[seq->gap_pos]=0; /* null terminate the first half */

	DEBUG("gap_pos=%zu pos=%zu gaplen=%zu len=%zu\n", seq->gap_pos, position, seq->gap_len, seq->len);

	DEBUG("PART0: \"%s\"\n", seq->data);
	TRACE_EXPR(seq->gap_pos);
	TRACE_EXPR(seq->gap_len);
	TRACE_EXPR(seq->alloc);
	TRACE_EXPR(seq->len);
	DEBUG("PART1: \"%s\"\n", &seq->data[seq->gap_pos+seq->gap_len]);
	return 1;
}

/* returns the buffer as a flat buffer */
const char *textseq_flattened(struct textseq *seq, size_t *len) {
	/* FYI: seq->gas_pos is the length of the first half */
	if(seq->gap_pos<seq->len) { /* check which half is smaller */
		/* top half is smaller, move data DOWN (and gap to start) */
		DEBUG("Top half is smaller. move DOWN\n");
		textseq_move_gap(seq, 0);
		if(len)
			*len=seq->len;
		assert(seq->gap_pos==0);
		return &seq->data[seq->gap_len]; /* assumes gap_pos is 0 */
	} else {
		/* bottom half is smaller, move data UP (and gap to end) */
		DEBUG("bottom half is smaller, move data UP (and gap to end)\n");
		textseq_move_gap(seq, textseq_cursor_eof(seq));
		if(len)
			*len=seq->len;
		assert(seq->gap_pos==seq->len);
		return seq->data;
	}
}

/* inserts a buffer before position, if len is (size_t)-1 then strlen(str) */
int textseq_insert(struct textseq *seq, size_t position, const char *str, size_t len) {
	assert(seq!=NULL);
	assert(str!=NULL);
	if(len==(size_t)-1)
		len=strlen(str);

	textseq_move_gap(seq, position);

	if(len>seq->gap_len) {
		if(!textseq_expand_gap(seq, len)) {
			abort(); /* should not fail */
		}
	}

	assert((seq->gap_pos+len)<=seq->alloc);

	memcpy(&seq->data[seq->gap_pos], str, len);
	seq->gap_pos+=len;
	seq->gap_len-=len;
	seq->len+=len;

	/* update null terminator */
	if(seq->gap_len)
		seq->data[seq->gap_pos]=0;

	return 1;
}

/* deletes text after position - if len is (size_t)-1 then delete to EOF */
/* TODO: get rid of the (size_t)-1 and just make delete use a range of positions */
int textseq_delete(struct textseq *seq, size_t position, size_t len) {
	assert(seq!=NULL);

	textseq_move_gap(seq, position);

	if(len==(size_t)-1)
		len=seq->len-position;
	assert(len<=seq->len-position); /* deleted too much */
	seq->gap_len+=len;
	seq->len-=len;
	assert(seq->gap_pos<=seq->gap_len);
	seq->data[seq->gap_pos]=0; /* update null terminator */

	return 1;
}

/* clears the entire sequence */
int textseq_clear(struct textseq *seq) {
	assert(seq!=NULL);
	seq->gap_pos=0;
	seq->gap_len=seq->alloc-1; /* leave room for null terminator on end */
	seq->len=0;

#ifndef NDEBUG
	/* fill the gap with junk data */
	memset(&seq->data[seq->gap_pos], '^', seq->gap_len);
#endif
	seq->data[seq->gap_pos]=0; /* null terminate the start of the gap */

	/* TODO: free() then malloc() a new buffer if it is considered "large" */
	return 1;
}

/* returns a unsigned character as an int using a VIRTUAL cursor position
 * negative -1 if outside of the range */
int textseq_getch(struct textseq *seq, unsigned cursor) {
	assert(seq!=NULL);
	if(cursor>=seq->len) {
		DEBUG("cursor %d exceeds range 0..%d\n", cursor, seq->len-1);
		return -1; /* exceeds range */
	}
	if(cursor<seq->gap_pos) {
		return (unsigned char)seq->data[cursor];
	} else {
		return (unsigned char)seq->data[cursor+seq->gap_len];
	}
}

/**************************************************************************/
/* Editor */

/* state for string editor */
struct editor_info {
	struct textseq seq;
	void *extra; /* passed to callback functions */
	/* call this when user saves - used to sync to database */
	void (*save_cb)(void *extra, char *str);
	/* call this when the user aborts - used to unlock */
	void (*abort_cb)(void *extra);
	int mode; /* 0=append to EOF, 1=insert at cursor */
	unsigned cursor;
};

void editor_help(void) {
	printf(
		".c - Clears text editor buffer\n"
		".f - Formats text editor buffer with indents on paragraphs (preserves line breaks)\n"
		".sp - spell checks text editor buffer\n"
		".s(?) - searches text editor buffer for strings matching query (possibly moves to the string?)\n"
		".r <parameter1> <parameter2> - replaces 1 occurance of parameter1 string with parameter2 string.\n"
		".rf - replaces all occurances of parameter1 string with parameter2 string.\n"
		".i <parameter1> <parameter2> - searches for parameter1 and inserts parameter2 before it.\n"
		".d <parameter1> - deletes the string specified in parameter 1\n"
		".n - turns on line numbers\n"
		".dl# - deletes a line specified by number\n"
		".l# - skips to the line specified by number\n"
		".p - prints contents\n"
		".q - exits without saving changes\n"
		".qs - saves changes and exits text editor\n"
		".a<#> - Appends a new line at #. If no # is specified, adds new line to end.\n"
		".h - help\n"
		".t <line1> <line2> - copy/pastes an existing line to the specified line.\n"
	);
}

int editor_init(struct editor_info *ei, void (*save_cb)(void *extra, char *str), void (*abort_cb)(void *extra), const char *str) {
	memset(ei, 0, sizeof *ei);
	if(!textseq_init(&ei->seq, str))
		return 0;
	ei->save_cb=save_cb;
	ei->abort_cb=abort_cb;
	return 1;
}

int editor_start(struct editor_info *ei, void (*save_cb)(void *extra, char *str), void (*abort_cb)(void *extra), const char *str) {
	if(!editor_init(ei, save_cb, abort_cb, str)) {
		return 0;
	}
	ei->cursor=0;
	ei->mode=0; /* start in append mode */

	printf("Enter .h for help:\n");

	return 1;
}

int editor_line(struct editor_info *ei, const char *str) {
	char *endptr;
	long tmpnum;

	assert(ei!=NULL);
	assert(str!=NULL);

	if(!str)
		return 0;
	if(str[0]=='.') { /* found a command */
		switch(str[1]) {
			case 'a':
				tmpnum=strtol(str+2, &endptr, 10);
				if(endptr==str+2) {
					/* no parameters found */
					ei->cursor=0;
					ei->mode=0; /* follow EOF mode */
				} else {
					/* user provided a numeric parameter */
					ei->cursor=textseq_cursor_lineat(&ei->seq, tmpnum);
					ei->mode=1; /* cursor mode */
				}
				/* TODO: check for a text parameter and appends the text */
				break;
			case 'c':
				textseq_clear(&ei->seq);
				break;
			case 'p':
				puts(textseq_flattened(&ei->seq, 0));
				break;
			case 'q':
				return 0; /* done */
				break;
			default:
				printf("Unknown command '.%c'\n", str[1]);
			case 'h':
				editor_help();
				break;
		}
	} else {
		if(ei->mode) {
			/* append to cursor */
			textseq_insert(&ei->seq, ei->cursor, str, (size_t)-1);
			/* TODO: update cursor position */
		} else {
			/* append line to the end */
			textseq_insert(&ei->seq, textseq_cursor_eof(&ei->seq), str, (size_t)-1);
		}
	}
	return 1;
}

/* adds a newline to the end of the buffer if one is not already there */
void editor_fix_nl(struct editor_info *ei) {
	unsigned cur;
	int ch;

	cur=textseq_cursor_charateof(&ei->seq);
	if(cur) {
		ch=textseq_getch(&ei->seq, cur-1);
		if(ch>=0 && ch!='\n') {
			DEBUG("Adding newline at %d\n", cur);
			textseq_insert(&ei->seq, cur, "\n", 1);
		}
	}
}

const char *testseq_test_str[] = {
	"Alas, poor Yorick!--I knew him, Horatio;\n"
	"a fellow of infinite jest, of most excellent fancy:\n"
	"he hath borne me on his back a thousand times;\n"
	"and now, how abhorred in my imagination it is!\n"
	"my gorge rises at it."
};

void editor_test(void) {
	struct editor_info es;
	char buf[512];
	const char *tmpstr;
	size_t tmplen;

	editor_start(&es, 0, 0, testseq_test_str[0]);
	editor_fix_nl(&es);
	while(fgets(buf, sizeof buf, stdin)) {
		if(!editor_line(&es, buf)) {
			break;
		}
	}

	tmpstr=textseq_flattened(&es.seq, &tmplen);
	printf("len=%zd\n@%s@\n", tmplen, tmpstr);
	textseq_free(&es.seq);
}

int textseq_test_cursor_at_line(struct textseq *seq, unsigned line) {
	int cursor;
	DEBUG("gap=%zu gaplen=%zu len=%zu\n", seq->gap_pos, seq->gap_len, seq->len);

	cursor=textseq_cursor_lineat(seq, line);
	printf("cursor: %d (line=%d)\n", cursor, line);
	if(cursor<0) {
		printf("cannot find line %d\n", line);
		abort();
	}
	return cursor;
}

int textseq_test_cursor_at_eof(struct textseq *seq) {
	int cursor;
	DEBUG("gap=%zu gaplen=%zu len=%zu\n", seq->gap_pos, seq->gap_len, seq->len);

	cursor=textseq_cursor_eof(seq);
	printf("cursor: %d (EOF)\n", cursor);
	assert(cursor>=0);
	return cursor;
}

void textseq_test(void) {
	struct textseq seq;
	int cursor;
	const char *tmpstr;
	size_t tmplen;

	if(!textseq_init(&seq, testseq_test_str[0]))
		abort();

	cursor=textseq_test_cursor_at_line(&seq, 2);
	textseq_move_gap(&seq, cursor); /* move up */
	textseq_dump(&seq, __LINE__);

	cursor=textseq_test_cursor_at_line(&seq, 2);
	textseq_move_gap(&seq, cursor); /* move to current position just to see what happens */
	textseq_dump(&seq, __LINE__);

	cursor=textseq_test_cursor_at_eof(&seq);
	textseq_move_gap(&seq, cursor); /* move gap to EOF */
	textseq_dump(&seq, __LINE__);

	cursor=textseq_test_cursor_at_line(&seq, 0);
	textseq_move_gap(&seq, cursor); /* move gap to start of file */
	textseq_dump(&seq, __LINE__);

	cursor=textseq_test_cursor_at_line(&seq, 0);
	textseq_insert(&seq, cursor, "[zero alpha]\n", (size_t)-1);
	textseq_dump(&seq, __LINE__);

	cursor=textseq_test_cursor_at_line(&seq, 4);
	printf("line length : %d\n", textseq_linelength(&seq, cursor));
	textseq_insert(&seq, cursor, "[one beta]\n", (size_t)-1);
	textseq_dump(&seq, __LINE__);

	cursor=textseq_test_cursor_at_line(&seq, 1);
	textseq_delete(&seq, cursor, 19);
	textseq_dump(&seq, __LINE__);

	tmpstr=textseq_flattened(&seq, &tmplen);
	printf("len=%zd\n@%s@\n", tmplen, tmpstr);

	textseq_free(&seq);
}

int main(void) {
	textseq_test();
	editor_test();

	return 0;
}
