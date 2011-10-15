/* twok-0.10
   Public domain
   Python-styled native-compiling scripting language
   http://h4ck3r.net/#twok

   Scott Graham 2011 <scott.twok@h4ck3r.net>
   No warranty implied; use at your own risk.


Before including,

    #define TWOK_DEFINE_IMPLEMENTATION

in *one* C file that you want to contain the implementation.


ABOUT:

    Native compile on x64, ARM (not yet), or interpreted (not yet)
    <2k LOC implementation (`sloccount twok.h`)
    No external dependencies

    ("twok" is a reference to the lines of code for the implementation, but currently
    only ~1k).


TODO:

    refactor asm generation
    funcs:
        print
        enumerate (need tuple unpack for that...)
        zip
        format?
        sorted
    simple HTTP POST-based repl (exists, doesn't do anything)
    fix '<tok>' expected error to print something useful
    closures?
    JIT debugging integration with GDB 7.1+
    *splat at callsite
    maybe derived (contained) structs
    list comprehensions
    pack for passing utf-8 back to C
    arm backend (on android ndk maybe)
    uninit var tracking (maybe optional?)
    more tests for various math/expr ops

    dupe getReg calls in g_rval
    share genlocal and atom.T_IDENT
    varargs code has duplication and ickiness

    only way to dereference right now is blah[0] which is ugly
    probably unary * would do, but wait to see how structs might work
    perhaps fixable by macro? might be too complicated for the macros
   
    macros:

        mac push(L, i):
            return [|
                listaddr_push(@$(L), $(i))
            |]

        hmm, but what about actual processing?

        mac push_up_to(L, i):
            for j in range(i):
                return [|
                    listaddr_push(@$(L), $(j))
                |]

        and how does indent/dedent work?

        probably return a list of [| |] blocks
        possibly use { } instead since we don't have dict



NOTES: (mostly mumbling about internal implementation details)

    functions
        - indirected through global table for hotpatching
        - just add all interned names of functions to a list and use the index
          in that list as func identifier (hash won't work so well)
    need to know in body of function if name is a (global) function or not
        - if assigned (anywhere in the body) it's a local for the whole body
        - otherwise, it's a global function. only functions for now.
        - hmm, scan sucks. how about: global if already defined, otherwise local
          (works for funcs, except fwddecl, allow def f(): pass at some point)
    logic ops, and or:
        - allocate a stack tmp
        - store the value to check into tmp then branch if nz/z
        - reload stack for TOS at end of all or/ands
        - keeping reg alloc working is tricky because running through code in
          straight line, so TOS doesn't mirror branching or bool ops. using
          stack means that the registers/vst aren't affected outside each arm
          of the or/and conditions.
    C function calls and runtime lib
        - manually exported currently
        - imported using "extern". functions can be passed in/out as pointers
          and share abi with host's C.
    logical not: just == 0 and back into reg
    math functions, + - * / & | ^
    unary ops
    parens for precedence

    externs:
        - would be nice to dlsym externs automatically, but then we'd have to
          scan the body of functions to know what was assigned to, rather than
          just used.
        - we could either do that, or require explicit 'extern blah'
          declarations at global scope?
        - can't just dlsym first because then whatever was imported into the C
          program might override globals and locals of the program which would
          be stupid.
        - blech, dlsym sucks. can't get stuff from current elf unless you add
          -rdynamic to the command line. can load from clib or other .so, but,
          meh.

    function call args
        - follow the abi for the platform. x64 is unfortunately different
        between microsoft and linux/osx (which follow amd's). they have a
        different number of reg args and msft reserves spill locations for the
        register args.
        - we want to interop easily w/ C, so use this for internal functions
        too (unfortunately)

    stack layout on x64, each is REG_SIZE(=8) big

        higher numbers
        +-------------------------
        | extra (non-reg) arg 3
        +-------------------------
        | extra arg 2
        +-------------------------
        | extra arg 1
        +-------------------------
        | extra arg 0
        +-------------------------
        | return addr
        +-------------------------
        | prev rbp         <-- RBP
        +-------------------------
        | linear arg copies
        | ...
        +-------------------------
        | locals/spills
        | ...
        +-------------------------
        lower numbers

        so,
        locals are rbp-8, -16, -24, ...
        extra args are rbp+8, +16, +24, ...

        for simplicity, on entry to function, we extend rsp and copy args into
        linear place to lookup. first 4 to 6 are in registers depending on abi.
        so,
        - rbp-8 is arg 0
        - rbp-16 is arg 1
        - rbp-24 is arg 2
        ...
        and,
        - rbp-N is local 0
        - rbp-N+8 is local 1
        ...
   
    mempush/pop for 'gc'

        very simple gc, while allowing for pointer futzing.
        mempush() marks a spot in a bump allocator, mempop() returns there.
        in debug, memset everything that's free'd to track errors down
        basically 2 stacks, the call stack and the alloc stack
        manual malloc/free tbd, not sure of a good way to hook up
        automatically allocating things to use manual instead (i.e. lists)

    strings (just different syntax for [])
        - decodes from utf-8 in source file to word-sized list at runtime
        - no pack yet for passing back to C as utf-8
    range func
        - only one arg, not various python since we don't have varargs (yet?)
    for x in blah
        - blah is always a list (as per range(), or [] synax)

    *args in function parameters:
        def blah(a, b, *args)
    takes 2+ params and args is an array of the rest of them.

    the called function constructs the args array on entry. in order to know
    how many arguments were passed, all calls set a hidden parameter (r10 on x64)
    'al' is used for this on sysv amdx64, but only represents the number of vector
    regs used, not the total num args. msft doesn't have any indication.

    unfortunately, this convention means that *args funcs aren't callable from C
    but oh well. could probably write a mini-forwarding function that sets r10 and
    then jumps to the real function.


    structs:

        class Stuff: x, y, z
        makes function Stuff(a,b,c) which pushes @Stuff,0,a,b,c onto list and returns it

            struct Stuff: x, y, z
            def __main__():
                v = Stuff(6, 7, 8)
                return v.Stuff_x + v.Stuff_y

        can't be just .x and .y without doing a runtime lookup, e.g.:
            struct A: x, y
            struct B: y, x
        then
            def doit(val):
                # what's this return?
                return val.x

        Also, Stuff? which is a predicate function. There's no 'derived' types yet.


*/

#ifndef INCLUDED_TWOK_H
#define INCLUDED_TWOK_H

#ifdef __cplusplus
extern "C" {
#endif

extern int twokRun(char *code, void *(*externLookup)(char *name));
extern void twokHttpRepl(void *(*externLookup)(char *name));

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_TWOK_H */

#ifdef TWOK_DEFINE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#if __unix__ || (__APPLE__ && __MACH__)
    #include <sys/mman.h>
    static void* twok_allocExec(int size) { void* p = mmap(0, size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0); memset(p, 0x90, size); return p; }
    static void twok_freeExec(void* p, int size) { munmap(p, size); }
    static int twok_CTZ(int x) { return __builtin_ctz(x); }
#elif _WIN32
    #if _M_PPC
    #else
        #include <windows.h>
        static void* twok_allocExec(int size) { void* p = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); memset(p, 0x90, size); return p; }
        static void twok_freeExec(void* p, int size) { VirtualFree(p, size, MEM_RELEASE); }
        #pragma intrinsic(_BitScanForward)
        static int twok_CTZ(int x) { unsigned long ret; _BitScanForward(&ret, x); return ret; }
        #define strtoll _strtoi64
        #ifdef _MSC_VER
            #pragma warning(disable: 4324 4053 4204 4127 4244 4305 4152 4055 4706 4702)
        #endif
    #endif
#endif

/* this is the GDB<->JIT interface */
typedef enum { JIT_NOACTION = 0, JIT_REGISTER_FN, JIT_UNREGISTER_FN } jit_actions_t;
struct jit_code_entry {
    struct jit_code_entry *next_entry;
    struct jit_code_entry *prev_entry;
    const char *symfile_addr;
    uint64_t symfile_size;
};

/* Make sure to specify the version statically, because the
debugger may check the version before we can set it.  */
struct jit_descriptor {
    uint32_t version;
    /* This type should be jit_actions_t, but we use uint32_t
       to be explicit about the bitwidth.  */
    uint32_t action_flag;
    struct jit_code_entry *relevant_entry;
    struct jit_code_entry *first_entry;
} __jit_debug_descriptor = { 1, 0, 0, 0 };
     
/* GDB puts a breakpoint in this function.  */
void __attribute__((noinline)) __jit_debug_register_code() { };

typedef struct Token {
    int type, pos;
    union {
        char* str;
        long long tokn;
    } data;
} Token;

typedef struct Value {
    union {
        unsigned long long _;
        int type;
        void (*handler)(struct Value*);
    } tag;
    union {
        unsigned long long _;
        int i;
        long long l;
        char* p;
    } data;
    int label;
} Value;
#define VAL(t, d) do { Value _ = { { (t) }, { (d) }, 0xbad1abe1 }; tvpush(C.vst, _); } while(0)
#define J_UNCOND 2
#define tarrsize(a) ((int)(sizeof(a)/sizeof((a)[0])))

typedef struct Context {
    Token *tokens;
    int curtok, *accessorOffsets;
    char *input, *codeseg, *codesegend, *codep, **strs, **locals, **funcnames, **funcaddrs, **externnames, **externaddrs, **accessorNames;
    void *(*externLookup)(char *name);
    Value *instrs, *vst;
    jmp_buf errBuf;
    char errorText[512];
} Context;


static Context C;
static void suite();
static int or_test();
static int atomplus();
static void *stdlibLookup(char *name);
static void error(char *fmt, ...);

/*
 * misc utilities.
 */

/* bump allocator with save/restore checkpoint stack */
static char *tba_stack[16];
static int tba_stackidx;
#ifndef TWOK_HEAP_SIZE
    #define TWOK_HEAP_SIZE (1<<20)
#endif
static char tba_heap[TWOK_HEAP_SIZE];
static char *tbap = tba_heap;
/* stored as size_t at -1 of returned pointer. if realloc is smaller,
 * return same, pointer, if it's bigger then always copy to end */
#define tba_blocksize(p) (*(((size_t*)(p))-1))
/* dumb validate mem, walks heap to make sure sizes look reasonable */
/*
static void tba_check_sizes()
{
    char *p = &tba_heap[0] + sizeof(size_t);
    for (;;)
    {
        if (p >= tbap) break;
        size_t size = tba_blocksize(p);
        if (size >= 10000)
            error("too big? %p", p);
        p += size + sizeof(size_t);
    }
}
*/
static void *tba_realloc(void *ptr, size_t size) {
    char *ret = tbap + sizeof(size_t);

    /* if there was a previous block, and we're not freeing, and it's smaller,
     * just return, there's nothing to do */
    if (ptr != NULL && size > 0 && tba_blocksize(ptr) >= size) return ptr;

    /* we don't actually do frees */
    if (size == 0) return NULL;

    /* now we know it's growing (possibly from null) */

    /* align to word-sized */
    size = (size + sizeof(size_t) - 1) & (~(sizeof(size_t) - 1));

    /* make sure we have enough space */
    if (tbap + size + sizeof(size_t) > &tba_heap[0] + (TWOK_HEAP_SIZE)) error("out of memory");

    /* store size of newly allocated block, and bump pointer */
    *(((size_t*)ret)-1) = size;
    tbap += size + sizeof(size_t);

    /*
    printf("allocating at %p, size %ld\n", ret, size);
    if (ptr)
        printf("  was at %p, size %ld\n", ptr, tba_blocksize(ptr));
    printf(" tbap now %p\n", tbap);
    */
    /* make a copy of the old data if there was any */
    if (ptr)
        memcpy(ret, ptr, tba_blocksize(ptr) < size ? tba_blocksize(ptr) : size);

    return ret;
}
#define TWOK_FILL_MEM 1
static void tba_pushcheckpoint() {
    if (tba_stackidx >= tarrsize(tba_stack)) error("too many checkpoints");
    tba_stack[tba_stackidx++] = tbap;
#ifdef TWOK_FILL_MEM
    memset(tbap, 0xcc, TWOK_HEAP_SIZE - (tbap - &tba_heap[0]));
#endif
}
static void tba_popcheckpoint() {
    if (tba_stackidx <= 0) error("checkpoint underflow");
    tbap = tba_stack[--tba_stackidx];
#ifdef TWOK_FILL_MEM
    memset(tbap, 0xcc, TWOK_HEAP_SIZE - (tbap - &tba_heap[0]));
#endif
}

/* simple vector based on http://nothings.org/stb/stretchy_buffer.txt */
#define tvfree(a)                   ((a) ? (tba_free(tv__tvraw(a)),(void*)0) : (void*)0)
#define tvpush(a,v)                 (tv__tvmaybegrow(a,1), (a)[tv__tvn(a)++] = (v))
#define tvpop(a)                    (((tv__tvn(a) > 0)?((void)0):error("assert")), tv__tvn(a)-=1)
#define tvsize(a)                   ((a) ? tv__tvn(a) : 0)
#define tvadd(a,n)                  (tv__tvmaybegrow(a,(int)(n)), tv__tvn(a)+=(n), &(a)[tv__tvn(a)-(n)])
#define tvlast(a)                   ((a)[tv__tvn(a)-1])
#define tvindexofnp(a,i,n,psize)    (tv__tvfind((char*)(a),(char*)&(i),sizeof(*(a)),n,psize))
#define tvindexof(a,i)              ((a) ? (tv__tvfind((char*)(a),(char*)&(i),sizeof(*(a)),tv__tvn(a),sizeof(*(a)))) : -1)
#define tvcontainsnp(a,i,n,psize)   ((a) ? (tv__tvfind((char*)(a),(char*)&(i),sizeof(*(a)),n,psize)!=-1) : 0)
#define tvcontainsp(a,i,psize)      (tvcontainsnp((a),i,tv__tvn(a),psize))
#define tvcontainsn_nonnull(a,i,n)  (tv__tvfind((char*)(a),(char*)&(i),sizeof(*(a)),n,sizeof(*(a)))!=-1) /* workaround for stupid warning */
#define tvcontains(a,i)             ((a) ? (tvcontainsp((a),i,sizeof(*(a)))) : 0)

#define tv__tvraw(a) ((int *) (a) - 2)
#define tv__tvm(a)   tv__tvraw(a)[0]
#define tv__tvn(a)   tv__tvraw(a)[1]

#define tv__tvneedgrow(a,n)  ((a)==0 || tv__tvn(a)+n >= tv__tvm(a))
#define tv__tvmaybegrow(a,n) (tv__tvneedgrow(a,(n)) ? tv__tvgrow(a,n) : 0)
#define tv__tvgrow(a,n)  tv__tvgrowf((void **) &(a), (n), sizeof(*(a)))

static void tv__tvgrowf(void **arr, int increment, int itemsize) {
    int m = *arr ? 2*tv__tvm(*arr)+increment : increment+1;
    void *p = tba_realloc(*arr ? tv__tvraw(*arr) : 0, itemsize * m + sizeof(int)*2);
    if (p) {
        if (!*arr) ((int *) p)[1] = 0;
        *arr = (void *) ((int *) p + 2);
        tv__tvm(*arr) = m;
    }
}
static int tv__tvfind(char* arr, char* find, int itemsize, int n, int partialsize) {
    int i;
    for (i = 0; i < n; ++i)
        if (memcmp(&arr[i*itemsize], find, partialsize) == 0)
            return i;
    return -1;
}

#define PREVTOK (C.tokens[C.curtok - 1])
#define CURTOK (&C.tokens[C.curtok])
#define CURTOKt (CURTOK->type)

static void geterrpos(int offset, int* line, int* col, char** linetext) {
    char* cur = C.input;
    *line = 1; *col = 1;
    *linetext = cur;
    while (--offset >= 0) {
        *col += 1;
        if (*cur++ == '\n') {
            *linetext = cur;
            *line += 1;
            *col = 1;
        }
    }
}

/* report error message and longjmp */
static void error(char *fmt, ...) {
    va_list ap;
    int line, col;
    char* text, *eotext;
    char tmp[256];

    va_start(ap, fmt);
    if (C.curtok < tvsize(C.tokens)) { /* for errors after parse finished */
        geterrpos(CURTOK->pos, &line, &col, &text);
        sprintf(C.errorText, "line %d, col %d:\n", line, col);
        eotext = text;
        while (*eotext != '\n' && *eotext != 0) ++eotext;
        strncat(C.errorText, text, eotext - text + 1);
        while (--col) strcat(C.errorText, " "); /* todo; ! */
        strcat(C.errorText, "^\n");
    }
    vsprintf(tmp, fmt, ap);
    strcat(C.errorText, tmp);
    strcat(C.errorText, "\n");
    longjmp(C.errBuf, 1);
}

/*
 * tokenize. build a tv of Token's for rest. indent/dedent is a bit icky.
 */

/* todo; True, False, None? */
static char KWS[] = " if elif else or for def return extern class in and not print pass << >> <= >= == != ";
static struct { char from, to; } strEscapes[] = {{'\\','\\'}, {'\'','\''}, {'"','"'}, {'b','\b'},{'r','\r'},{'t','\t'},{'n','\n'},{'0','\0'}};
#define KW(k) ((int)((strstr(KWS, #k " ") - KWS) + T_KW))
static char* strintern(char* s) {
    int i;
    char *copy = 0;
    for (i = 0; i < tvsize(C.strs); ++i)
        if (strcmp(s, C.strs[i]) == 0)
            return C.strs[i];
    while (*s) tvpush(copy, *s++);
    tvpush(copy, 0);
    tvpush(C.strs, copy);
    return tvlast(C.strs);
}
enum { T_UNK, T_KW=1<<7, T_IDENT = 1<<8, T_END, T_NL, T_NUM, T_STR, T_INDENT, T_DEDENT };
#define TOK(t) do { Token _ = { t, (int)(startpos - C.input), { strintern(#t) } }; tvpush(C.tokens, _); } while(0)
#define TOKI(t, s) do { Token _ = { t, (int)(startpos - C.input), { strintern(s) } }; tvpush(C.tokens, _); } while(0)
#define TOKN(t, v) do { Token _ = { t, (int)(startpos - C.input), { 0 } }; _.data.tokn=v; tvpush(C.tokens, _); } while(0)
#define isid(ch) (isalnum(ch) || ch == '_' || ch == '?')

static void tokenize() {
    char *pos = C.input, *startpos;
    int i, tok, column;
    int *indents = 0;
    char *ident = 0, *tempident = 0;

    while (*pos) {
        if (*pos == '#')
            while (*pos && *pos != '\n') *pos++ = ' ';
        else pos++;
    }

    pos = C.input;

    tvpush(indents, 0);

    for (;;) {
        column = 0;
        while (*pos == ' ') { pos++; column++; }
        startpos = pos;

        if (*pos == 0) {
donestream: for (i = 1; i < tvsize(indents); ++i)
                TOK(T_DEDENT);
            TOK(T_END);
            return;
        }

        if (*pos == '\r' || *pos == '\n') {
            ++pos;
            continue;
        }

        while (column < tvlast(indents)) {
            if (!tvcontains(indents, column)) error("unindent does not match any outer indentation level");
            tvpop(indents);
            TOK(T_DEDENT);
        }
        if (column > tvlast(indents)) {
            tvpush(indents, column);
            TOK(T_INDENT);
        }

        while (*pos != '\n') {
            while (*pos == ' ') ++pos;
            if (*pos == '\n') break;
            startpos = pos;
            ident = NULL;
            tok = *pos;
            if (isid(*pos)) {
                while (isid(*pos))
                    tvpush(ident, *pos++);
                tvpush(ident, 0);
                if (isdigit(tok))
                    TOKN(T_NUM, strtoll(ident, 0, 0));
                else {
                    /* oops, need to search with space before/after so "i"
                     * isn't found in "if" and "x" isn't found in "extern". */
                    tvpush(tempident, ' ');
                    for (i = 0; i < tvsize(ident) - 1; ++i) tvpush(tempident, ident[i]); /* don't inc nul */
                    tvpush(tempident, ' ');
                    tvpush(tempident, 0);
                    tok = T_IDENT;
                    if (strstr(KWS, tempident)) tok = (int)(strstr(KWS, tempident) + 1 /*space*/ - KWS + T_KW);
                    TOKI(tok, ident);
                    tempident = NULL;
                }
            } else if (*pos == '\'' || *pos == '\"') {
                char *str = 0;
                while (*++pos != *startpos) {
                    if (*pos == '\\') {
                        ++pos;
                        for (i = 0; i < tarrsize(strEscapes); ++i)
                            if (strEscapes[i].from == *pos)
                                tvpush(str, strEscapes[i].to);
                    } else tvpush(str, *pos);
                }
                ++pos;
                tvpush(str, 0);
                TOKI(T_STR, str);
            } else {
                char tmp[3] = { 0, 0, 0 };
                if (!*pos) goto donestream;
                tmp[0] = *pos++;
                if (0) {}
                #define twochar(t) else if (*(pos-1) == #t[0] && *pos == #t[1]) { tmp[1] = *pos++; TOKI(KW(t), tmp); }
                    twochar(<<) twochar(>>)
                    twochar(<=) twochar(>=)
                    twochar(!=) twochar(==)
                #undef twochar
                else TOKI(tmp[0], tmp);
            }
        }
        TOK(T_NL);
        ++pos;
    }
}

/*
 * backends, define one of them.
 */

/* x64 backend */
#if defined(_M_X64) || defined(__amd64__)

/* currently used: rax, rcx, rdx, rsi, rdi; all volatile across calls.
 * hmm. actually difficult to construct basic math ops that use more than 4
 * regs anyway, so not worth it straight away. */
enum { V_TEMP=0x1000, V_ISADDR=0x2000, V_LOCAL=0x4000, V_FUNC=0x8000, V_IMMED=0x10000, V_WANTADDR=0x20000,
       V_REG_RAX=0x0001, V_REG_RCX=0x0002, V_REG_RDX=0x0004, V_REG_RBX=0x0008,
       V_REG_RSP=0x0010, V_REG_RBP=0x0020, V_REG_RSI=0x0040, V_REG_RDI=0x0080,
       V_REG_R8=0x0100, V_REG_R9=0x0200, V_REG_R10=0x0400, V_REG_R11=0x0800,
       V_REG_ANY=V_REG_RAX | V_REG_RCX | V_REG_RDX | V_REG_R8 | V_REG_R9, /* all volatile for all abis */
       V_REG_FIRST = V_REG_RAX, V_REG_LAST = V_REG_R11,
};

typedef long long tword;

/* bah. asshats used different abis for x64. */
static int funcArgRegs[] = {
#if __unix__ || (__APPLE__ && __MACH__)
    V_REG_RDI, V_REG_RSI, V_REG_RDX, V_REG_RCX, V_REG_R8, V_REG_R9
    #define EXTRASTACK 0
#elif _WIN32
    V_REG_RCX, V_REG_RDX, V_REG_R8, V_REG_R9
    #define EXTRASTACK 32
#endif
};


enum { REG_SIZE = 8, FUNC_THUNK_SIZE = 8 };

#define ob(b) (*C.codep++ = (b))
#define lead(r) ob(0x48 | ((r>=V_REG_R8)?1:0))
#define lead2(r1,r2) ob(0x48 | ((r1>=V_REG_R8)?1:0) | ((r2>=V_REG_R8)?4:0))
#define outnum32(n) { unsigned int _ = (unsigned int)(n); unsigned int mask = 0xff; unsigned int sh = 0; int i; for (i = 0; i < 4; ++i) { ob((char)((_&mask)>>sh)); mask <<= 8; sh += 8; } }
#define outnum64(n) { unsigned long long _ = (unsigned long long)(n); unsigned long long mask = 0xff; unsigned long long sh = 0; int i; for (i = 0; i < 8; ++i) { ob((char)((_&mask)>>sh)); mask <<= 8; sh += 8; } }

typedef struct NativeContext {
    int spills[64] /* is the Nth spill location in use? */, varargsgetsize;
    char *numlocsp, *varargsget, *symfile, **paramnames;
} NativeContext;
static NativeContext NC;

#define vreg_to_enc(vr) (((vr) >= V_REG_R8) ? twok_CTZ(((vr)>>8)) : twok_CTZ(vr))

#define put32(p, n) (*(int*)(p) = (n))
#define get32(p) (*(int*)p)

static void i_setup() {
    int i, stackdelta = 0 + EXTRASTACK;
    memset(&NC, 0, sizeof(NC));
    /* generate varargs helper code
       when we get into a function that has *args, we don't know how many more have been
       passed at compile time, and some of them may be in registers. by call'ing
       varargsget + i*varargsgetsize the i'th argument will be put into rax. this also
       handles the differences between abis for msft vs. sysv.

       code looks like:
            mov rax, funcArgRegs[0]
            ret
            mov rax, funcArgRegs[1]
            ret
            ...
            mov rax, [rbp + 16+8*0]
            ret
            mov rax, [rbp + 16+8*1]
            ret
            mov rax, [rbp + 16+8*2]
            ...
        with each set padded to be varargsgetsize (==8) big.
    */
    NC.varargsget = C.codep;
    NC.varargsgetsize = 8;
    for (i = 0; i < 64; ++i) { /* up to this many args to func, can be arbitrarily increased */
        if (i < tarrsize(funcArgRegs)) {
            lead2(V_REG_RAX, funcArgRegs[i]); ob(0x89); ob(0xc0 + vreg_to_enc(funcArgRegs[i]) * 8); /* mov rax, rXx */
        } else {
            ob(0x48); ob(0x8b); ob(0x85); outnum32(16+stackdelta+8*(i - tarrsize(funcArgRegs))); /* mov rax, [ebp + x] */
        }
        ob(0xc3); /* ret */
        while ((unsigned long long)C.codep % NC.varargsgetsize != 0) ob(0x90); /* pad with nop */
    }

    /* emit the header of the elf symbol file */
    #define sym_add64(x) do { unsigned long long v = x; memcpy(tvadd(NC.symfile, sizeof(v)), &v, sizeof(v)); } while(0)
    #define sym_add32(x) do { unsigned int v = x; memcpy(tvadd(NC.symfile, sizeof(v)), &v, sizeof(v)); } while(0)
    #define sym_add16(x) do { unsigned short v = x; memcpy(tvadd(NC.symfile, sizeof(v)), &v, sizeof(v)); } while(0)
    #define sym_add(lit) memcpy(tvadd(NC.symfile, sizeof(lit) - 1), lit, sizeof(lit) - 1)
    #define sym_ehdr_rest(entry, phoff, shoff, flags, ehsize, phentsize, phnum, shentsize, shnum, shstrndx)         \
            sym_add64(entry); sym_add64(phoff); sym_add64(shoff); sym_add32(flags); sym_add16(ehsize);              \
            sym_add16(phentsize); sym_add16(phnum); sym_add16(shentsize); sym_add16(shnum); sym_add16(shstrndx);
    #define sym_shdr(name, type, flags, addr, offset, size, link, info, addralign, entsize)                         \
            sym_add32(name); sym_add32(type); sym_add64(flags); sym_add64(addr); sym_add64(offset);                 \
            sym_add64(size); sym_add32(link); sym_add32(info); sym_add64(addralign); sym_add64(entsize);
    sym_add("\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\1\0\076\0\1\0\0\0");
    sym_ehdr_rest(0, 0, 0x40, 0, 0x40, 0, 0, 0x40, 5, 1);
    sym_shdr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    sym_shdr(1, 3, 0, 0, 384, 32, 0, 0, 1, 0); /* .shstrtab, SHT_STRTAB, ..., offset, length */
    sym_shdr(11, 1, 0, /*addr*/0, 0, 0, 0, 0, 0, 0);   /* .text, ... */
    sym_shdr(17, 1, 0, 0, /*offset*/0, /*size*/0, 0, 0, 0, 0); /* .stab, ... */
    sym_shdr(23, 3, 0, 0, /*offset*/0, /*size*/0, 0, 0, 0, 0); /* .stabstr, ... */
    /* .shstrtab */
    sym_add("\0.shstrtab\0.text\0.stab\0.stabstr\0");
}

static char* functhunkaddr(long long idx) { return C.codesegend - (idx + 1) * FUNC_THUNK_SIZE; }
static void addfunc(char* name, char* addr) {
    char* p;
    if (tvcontains(C.funcnames, name)) error("%s already defined", name);
    tvpush(C.funcnames, name);
    tvpush(C.funcaddrs, addr);
    p = functhunkaddr(tvsize(C.funcnames) - 1);
    *p++ = 0xe9; /* jmp relimmed */
    put32(p, (int)(addr - p - 4)); p += 4;
    *p++ = 0xcc; *p++ = 0xcc; *p++ = 0xcc; /* add int3 to rest of thunk */
}
static int funcidx(char* name) { char* p = strintern(name); return tvindexof(C.funcnames, p); }

/* store the given register (offset) into the given stack slot */
static void g_storespill(int reg, int slot) {
    lead2(0, reg); ob(0x89); ob(0x85 + vreg_to_enc(reg) * 8);
    outnum32((-1 - slot - tvsize(NC.paramnames) - tvsize(C.locals)) * REG_SIZE);
}

/* load spill # slot into reg */
static void g_loadspill(int reg, int slot) {
    lead(reg); ob(0x8b); ob(0x85 + vreg_to_enc(reg) * 8);
    outnum32((-1 - slot - tvsize(NC.paramnames) - tvsize(C.locals)) * REG_SIZE);
}

static void g_swap() {
    Value tmp = tvlast(C.vst);
    tvlast(C.vst) = C.vst[tvsize(C.vst) - 2];
    C.vst[tvsize(C.vst) - 2] = tmp;
}

static int getReg(int valid) {
    int i, j, reg;

    /* figure out if it's currently in use */
    for (i = V_REG_FIRST; i <= V_REG_LAST; i <<= 1) {
        if ((i & valid) == 0) continue;
        for (j = 0; j < tvsize(C.vst); ++j)
            if ((C.vst[j].tag.type & i))
                break;
        /* not in use, return this one */
        if (j == tvsize(C.vst))
            return i;
    }

    /* otherwise, find the oldest in the class */
    for (j = 0; j < tvsize(C.vst); ++j) {
        if ((C.vst[j].tag.type & valid)) {
            /* and a location to spill it to */
            for (i = 0; i < tarrsize(NC.spills); ++i)
                if (!NC.spills[i]) break;

            /* and send it there and update the flags */
            reg = C.vst[j].tag.type & valid;
            g_storespill(reg, i);
            NC.spills[i] = 1;
            C.vst[j].tag.type &= ~V_REG_ANY;
            C.vst[j].tag.type |= V_TEMP;
            C.vst[j].data.i = i;

            /* and return that register */
            return reg;
        }
    }
    error("internal error, or out of stack slots");
    return -1;
}

static int g_lval(int valid) {
    int reg = 0, tag = tvlast(C.vst).tag.type;
    long long val = tvlast(C.vst).data.l;
    if (tag & V_ISADDR) {
        if ((reg = (tag & V_REG_ANY))) { /* nothing, just pop and return reg */ }
        else if (tag & V_IMMED) {
            /* mov Reg, const */
            reg = getReg(valid);
            lead(reg); ob(0xb8 + vreg_to_enc(reg));
            outnum64(val);
        } else if (tag & V_LOCAL) {
            reg = getReg(valid);
            lead(reg); ob(0x8d); ob(0x85 + vreg_to_enc(reg) * 8); /* lea rXx, [rbp - xxx] (long form) */
            outnum32(val * REG_SIZE);
        }
    }
    else error("expecting lval");
    tvpop(C.vst);
    return reg;
}

static int g_rval(int valid) {
    int reg, reg2, tag = tvlast(C.vst).tag.type;
    long long val = tvlast(C.vst).data.l;
    if (tag & V_IMMED) {
        if (tag & V_WANTADDR) error("cannot take address of immediate");
        /* mov Reg, const */
        reg = getReg(valid);
        lead(reg); ob(0xb8 + vreg_to_enc(reg));
        outnum64(val);
    } else if (tag & V_LOCAL) {
        /* todo; uninit var; keep shadow stack of initialized flags, error on
         * read before write. need to figure out calling C lib */
        if (tag & V_WANTADDR) return g_lval(valid);
        reg = getReg(valid);
        lead(reg); ob(0x8b); ob(0x85 + vreg_to_enc(reg) * 8); /* mov rXx, [rbp + xxx] (long form) */
        outnum32(val * REG_SIZE);
    } else if (tag & V_FUNC) {
        reg = getReg(valid);
        lead(reg); ob(0xb8 + vreg_to_enc(reg)); outnum64(functhunkaddr(val)); /* mov rXx, functhunk */
    }
    else if ((reg = (tag & V_REG_ANY) & valid)) { /* nothing to do, just return register */ }
    else if ((reg2 = (tag & V_REG_ANY))) {
        /* in a register, but not the one we need */
        reg = getReg(valid);
        lead2(reg, reg2); ob(0x89); ob(0xc0 + vreg_to_enc(reg) + vreg_to_enc(reg2) * 8); /* mov rXx, rXx */
    } else if (tag & V_TEMP) {
        reg = getReg(valid);
        g_loadspill(reg, (int)val);
    }
    else error("internal error, unexpected stack state");
    tvpop(C.vst);
    return reg;
}

/* emit a jump, returns the location that needs to be fixed up. make a linked
 * list to previous items that are going to jump to the same final location so
 * that when the jump target is reached we can fix them all up by walking the
 * list that we created. */
static char* i_jmpc(int cond, char* prev) {
    if (cond == J_UNCOND) {
        ob(0xe9);
        outnum32(prev ? prev - C.codeseg : 0);
    } else {
        int reg = g_rval(V_REG_ANY);
        lead2(reg, reg); ob(0x85); ob(0xc0 + vreg_to_enc(reg) * 9); /* test rXx, rXx */
        ob(0x0f); ob(0x84 + cond); /* jz/jnz rrr */
        outnum32(prev ? prev - C.codeseg : 0);
    }
    return C.codep - 4;
}

static void i_push(int reg) { if (reg>=V_REG_R8) ob(0x41); ob(0x50+vreg_to_enc(reg)); }
static void i_pop(int reg) { if (reg>=V_REG_R8) ob(0x41); ob(0x58+vreg_to_enc(reg)); }

static void i_func(char *name, char **paramnames, int hasvarargs)
{
    int i;
    while (((unsigned long)C.codep) % 16 != 0) ob(0x90); /* nop to align */
    addfunc(name, C.codep);
    ob(0x55); /* push rbp */
    lead(0); ob(0x89); ob(0xe5); /* mov rbp, rsp */
    lead(0); ob(0x81); ob(0xec); outnum32(0); /* sub rsp, xxx */
    NC.numlocsp = C.codep - 4; /* save for endfunc to patch */
    NC.paramnames = paramnames;
    /* copy args to shadow location, we put them in "upside down" from how
     * they are on the arg stack */
    for (i = 0; i < tvsize(paramnames) - hasvarargs; ++i) {
        /* get either from reg, or from stack, depending on index and abi */
        if (i >= tarrsize(funcArgRegs)) {
            ob(0x48); ob(0x8b); ob(0x85); outnum32(16+8*i); /* mov rax, [rbp + argoffset] */
        } else {
            int reg = funcArgRegs[i];
            lead2(V_REG_RAX, reg); ob(0x89); ob(0xc0 + vreg_to_enc(V_REG_RAX) + vreg_to_enc(reg) * 8); /* mov rXx, rXx */
        }
        /* copy into local location */
        ob(0x48); ob(0x89); ob(0x85); outnum32(-8 - 8*i);   /* mov [rbp - copyoffset], rax */
    }
    if (hasvarargs) {
        /* build a list of args that aren't included in our prototype. there's
         * (r10 - tarrsize(paramnames) + 1), +1 because the *arg is included
         * in the paramnames list */
        char *listpush = strintern("listaddr_push"), *listrev = strintern("reverse"), *topofloop;
        int pushidx = tvindexof(C.externnames, listpush), revidx = tvindexof(C.externnames, listrev), listptroffset = -8 - 8*(tvsize(paramnames)-1);
        /* load null (empty list) */
        ob(0x48); ob(0xb8); outnum64(0);
        ob(0x48); ob(0x89); ob(0x85); outnum32(listptroffset);   /* mov [rbp - copyoffset], rax */

        ob(0x49); ob(0x81); ob(0xea); outnum32(tvsize(paramnames) - 1); /* sub r10, #named_params */
        /* r10 is now the number of *args, not their index */
        topofloop = C.codep;

        /* r10 is the number of args, as set by the caller
           r11 is used as the jump-to-address for varargsget, see i_setup
           rax is used to calculate to offset and as the return value to store into *args
           r10 is volatile, so we need to save when we call into listaddr_push, and
           we also need to save the 1st and 2nd func args so we can call listaddr_push
           but still get them for this function. */
        /* push all volatile regs that are used in func calls so that we can call
           the list push function in C, and also r10 */
        ob(0x41); ob(0x52); /* push r10 */
        for (i = 0; i < tarrsize(funcArgRegs); ++i) i_push(funcArgRegs[i]);

        /* get then Nth argument */
        ob(0x4c); ob(0x89); ob(0xd0); /* mov rax, r10 */
        /* convert from counter to index. first -1 removes *arg from
         * list, second is because counter is post-decremented */
        ob(0x48); ob(0x05); outnum32(tvsize(paramnames) - 1 - 1); /* add rax, N */
        ob(0x48); ob(0x6b); ob(0xc0); ob(0x08); /* imul rax, rax, byte +0x8 */
        ob(0x49); ob(0xbb); outnum64(NC.varargsget); /* mov r11, varargsget */
        ob(0x49); ob(0x01); ob(0xc3); /* add r11, rax */
        ob(0x41); ob(0xff); ob(0xd3); /* call r11 */

        /* call listaddr_push to add to the *args array */
        lead2(0, funcArgRegs[0]); ob(0x8d); ob(0x85 + vreg_to_enc(funcArgRegs[0]) * 8); outnum32(listptroffset); /* lea funcarg0, [ebp - copyoffset] */
        lead2(funcArgRegs[1], V_REG_RAX); ob(0x89); ob(0xc0 + vreg_to_enc(funcArgRegs[1])); /* mov funcarg1, rax */
        ob(0x49); ob(0xbb); outnum64(C.externaddrs[pushidx]); /* mov r11, pushidx */
        if (EXTRASTACK) { ob(0x48); ob(0x83); ob(0xec); ob(0x20); /* sub rsp, 0x20 */ }
        ob(0x41); ob(0xff); ob(0xd3); /* call r11 */
        if (EXTRASTACK) { ob(0x48); ob(0x83); ob(0xc4); ob(0x20); /* add rsp, 0x20 */ }

        /* restore volatile registers */
        for (i = tarrsize(funcArgRegs) - 1; i >= 0; --i) i_pop(funcArgRegs[i]);
        ob(0x41); ob(0x5a); /* pop r10 */

        ob(0x49); ob(0xff); ob(0xca);   /* dec r10 */
        ob(0x0f); ob(0x85); outnum32(topofloop - C.codep - 4); /* jnz top of copy loop */

        /* now the list is built, but it's reversed, reverse it in place */
        lead(funcArgRegs[0]); ob(0x8b); ob(0x85 + vreg_to_enc(funcArgRegs[0]) * 8); outnum32(listptroffset); /* mov funcarg0, [ebp - copyoffset] */
        ob(0x49); ob(0xbb); outnum64(C.externaddrs[revidx]); /* mov r11, revidx */
        if (EXTRASTACK) { ob(0x48); ob(0x83); ob(0xec); ob(0x20); /* sub rsp, 0x20 */ }
        ob(0x41); ob(0xff); ob(0xd3); /* call r11 */
        if (EXTRASTACK) { ob(0x48); ob(0x83); ob(0xc4); ob(0x20); /* add rsp, 0x20 */ }
    }
    VAL(V_IMMED, 0); /* for fall off ret */
}

static void i_extern(char* name) {
    void *p;
    name = strintern(name);
    p = C.externLookup(name);
    if (!p) p = stdlibLookup(name);
    if (!p) error("'%s' not found", name);
    if (tvcontains(C.externnames, name)) return; /* not an error, just ignore. */
    tvpush(C.externnames, name);
    tvpush(C.externaddrs, p);
}

static void i_ret() { g_rval(V_REG_RAX); ob(0xc9); /* leave */ ob(0xc3); /* ret */ }
static void i_endfunc() {
    i_ret();
    put32(NC.numlocsp, (tvsize(C.locals)+1) * REG_SIZE + 256); /* todo; XXX hardcoded # spills */
}

static void i_cmp(int op) {
    int a, into;
    struct { char kw, cc; } cmpccs[] = {
        { '<', 0xc },
        { '>', 0xf },
        { KW(<=), 0xe },
        { KW(>=), 0xd },
        { KW(==), 4 },
        { KW(!=), 5 },
    };
    a = g_rval(V_REG_RAX);
    into = g_rval(V_REG_ANY & (~a));
    lead(into); ob(0x39 + vreg_to_enc(a)); ob(0xc0 + vreg_to_enc(into)); /* cmp rXx, rax */

    lead(into); ob(0xb8 + vreg_to_enc(into));
    outnum64(0);
    ob(0x0f);
    ob(0x90 + cmpccs[tvindexofnp(cmpccs, op, 6, 1)].cc);
    ob(0xc0 + vreg_to_enc(into));
    VAL(into, 0);
}

static void i_label(char* p) {
    char* to = C.codep;
    while (p) {
        char* tmp = get32(p) ? get32(p) + C.codeseg : 0; /* next value in the list before we overwrite it */
        put32(p, (int)(to - p - 4));
        p = tmp;
    }
}

/* lhs, rhs on stack */
static void i_store() {
    int val = g_rval(V_REG_ANY);
    int into = g_lval(V_REG_ANY & (~val));
    lead2(val, into); ob(0x89);
    ob(vreg_to_enc(into) + vreg_to_enc(val) * 8);
}

static void i_addrparam(int loc, int addrof) { VAL(V_LOCAL | V_ISADDR | (addrof ? V_WANTADDR : 0), -loc - 1); }
static void i_addrlocal(int loc, int addrof) { VAL(V_LOCAL | V_ISADDR | (addrof ? V_WANTADDR : 0), -loc - tvsize(NC.paramnames) - 1); }
static void i_storelocal(int loc) {
    i_addrlocal(loc, 0);
    g_swap();
    i_store();
}
static void i_subscript() {
    int index = g_rval(V_REG_ANY);
    int arr = g_rval(V_REG_ANY & (~index));
    lead(index); ob(0xc1); ob(0xe0 + vreg_to_enc(index)); ob(twok_CTZ(sizeof(size_t))); /* shl rIx, 3 */
    lead2(arr, index); ob(0x01); ob(0xc0 + vreg_to_enc(arr) + vreg_to_enc(index) * 8); /* add rAx, rIx */
    lead(arr); ob(0x8b); ob(vreg_to_enc(arr) * 9);
    VAL(arr, 0);
}


static void i_call(int argcount) {
    int i, stackdelta = (argcount - tarrsize(funcArgRegs)) * 8, argnostack = 1;
    if (stackdelta < 0) stackdelta = 0 + EXTRASTACK;
#if _WIN32
    argnostack = 0;
#endif
    if (stackdelta > 0) {
        lead(0); ob(0x81); ob(0xec); outnum32(stackdelta);
    }

    /* we have them in reverse order (pushed L->R), so reverse index */
    for (i = 0; i < argcount; ++i) {
        int idx = argcount - i - 1;
        if (idx >= tarrsize(funcArgRegs)) {
            g_rval(V_REG_R11);
            /* mov [rsp+X], r11 */
            ob(0x4c); ob(0x89); ob(0x5c); ob(0x24); ob((idx - tarrsize(funcArgRegs)*argnostack) * 8);
        } else g_rval(funcArgRegs[idx]);
    }

    /* stuff argcount into r10 for *args receiving functions */
    ob(0x49); ob(0xba); outnum64(argcount);

    g_rval(V_REG_R11); /* al is used for varargs on amd64 abi, r11 is volatile for both */
    ob(0x41); ob(0xff); ob(0xd3); /* call r11 */

    if (stackdelta > 0) {
        lead(0); ob(0x81); ob(0xc4); outnum32(stackdelta); /* clean up */
    }
    VAL(V_REG_RAX, 0);
}

static void i_mathunary(int op) {
    int reg;
    if (op == '+') return;
    reg = g_rval(V_REG_ANY);
    /* either neg or not */
    lead(reg); ob(0xf7); ob(op == '-' ? 0xd8 : 0xd0 + vreg_to_enc(reg));
    VAL(reg, 0);
}

/* + - * / % & | ^ */
static void i_math(int op) {
    struct { char math, opc; } map[] = { /* maps KW to x64 instr */
        { '+', 0x01 },
        { '-', 0x29 },
        { '*', 0x0f }, /* actually 0f af */
        { '&', 0x21 },
        { '^', 0x31 },
        { '|', 0x09 } };
    int opi = tvindexofnp(map, op, 6, 1);
    if (opi >= 0 || op == '*') {
        int v1 = g_rval(V_REG_ANY);
        int v0 = g_rval(V_REG_ANY & ~v1);
        lead2(v0, v1); ob(map[opi].opc);
        if (op == '*') ob(0xaf);
        ob(0xc0 + vreg_to_enc(v0) + vreg_to_enc(v1) * 8);
        VAL(op == '*' ? v1 : v0, 0); /* bleh, extended imul args backwards? */
    } else if (op == '/' || op == '%') {
        int v1 = g_rval(V_REG_ANY & ~(V_REG_RAX | V_REG_RDX));
        g_rval(V_REG_RAX);
        lead(V_REG_RAX); ob(0x99); /* cqo (sign extend rax into rdx) */
        lead(v1); ob(0xf7); ob(0xf8 + vreg_to_enc(v1)); /* idiv rXx */
        VAL(op == '/' ? V_REG_RAX : V_REG_RDX, 0); /* quotient in A, remainder in D */
    }
}

#endif

/*
 * utf-8 decode. based Bjoern Hoehrmann's version
 */
static tword *utf8_decode(unsigned char *str) {
    static unsigned char decode[] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,204,204,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,174,158,158,158,158,158,158,158,158,158,158,158,158,142,126,126,111, 95, 95, 95, 79,207,207,207,207,207,207,207,207,207,207,207,
        0,1,1,1,8,7,6,4,5,4,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,8,7,6,4,5,4,3,2,1,1,1,1,
    };
    tword *ret = 0;
    unsigned char byte, data, stat = 9;
    unsigned int unicode = 0;
    while ((byte = *str++)) {
        data = decode[(int)byte];
        stat = decode[256 + (stat << 4) + (data >> 4)];
        byte = (byte ^ (unsigned char)(data << 4));
        unicode = (unicode << 6) | byte;
        if (stat == 0) {
            tvpush(ret, unicode);
            unicode = 0;
        }
        if (stat == 1) error("invalid utf-8");
    }
    return ret;
}


/*
 * parsing and intermediate gen
 */
#define NEXT() do { if (C.curtok >= tvsize(C.tokens)) error("unexpected end of input"); C.curtok++; } while(0)
#define SKIP(t) do { if (CURTOKt != t) error("'%c' expected, got '%s'", t, CURTOK->data.str); NEXT(); } while(0)

static int genlocal() {
    static int count = 0;
    char buf[128], *name;
    sprintf(buf, "$loc%d", count++);
    name = strintern(buf);
    if (!tvcontains(C.locals, name)) tvpush(C.locals, name);
    return tvindexof(C.locals, name);
}

static void addAccessor(char *basename, char *itemname, int index) {
    int i;
    char buf[256], *tmp;
    sprintf(buf, "%s_%s", basename, itemname);
    tmp = strintern(buf);
    for (i = 0; i < tvsize(C.accessorNames); ++i) if (C.accessorNames[i] == tmp) return;
    tvpush(C.accessorNames, tmp);
    tvpush(C.accessorOffsets, index);
}

static int atom() {
    char *listpush = strintern("listaddr_push"), *listrev = strintern("reverse");
    int pushidx = tvindexof(C.externnames, listpush), revidx = tvindexof(C.externnames, listrev);
    if (CURTOKt == '(') {
        NEXT();
        or_test();
        SKIP(')');
        return 1;
    } else if (CURTOKt == '[') {
        int initialCount = tvsize(C.vst);
        NEXT();
        or_test();
        for (;;) {
            if (CURTOKt == ']') {
                int i, numElems = tvsize(C.vst) - initialCount, listtmp = genlocal();
                NEXT();
                VAL(V_IMMED, 0);
                i_storelocal(listtmp);
                for (i = 0; i < numElems; ++i) {
                                                            /* stack: V */
                    VAL(V_IMMED, (unsigned long long)C.externaddrs[pushidx]);   /* stack: V, listaddr_push */
                    g_swap();                               /* stack: listaddr_push, V */
                    i_addrlocal(listtmp, 1);                /* stack: listaddr_push, V, @L */
                    g_swap();                               /* stack: listaddr_push, @L, V */
                    i_call(2);
                    tvpop(C.vst); /* discard */
                }
                /* they were parsed onto the stack and then pushed, so they're
                 * reversed, just reverse() the whole list now. kind of dumb,
                 * but simple. */
                VAL(V_IMMED, (unsigned long long)C.externaddrs[revidx]);
                i_addrlocal(listtmp, 0);
                i_call(1);
                tvpop(C.vst); /* discard */
                /* return */
                i_addrlocal(listtmp, 0);
            }
            else if (CURTOKt == ',') {
                NEXT();
                or_test();
            } else break;
        }
    } else if (CURTOKt == T_NUM) {
        VAL(V_IMMED, CURTOK->data.tokn);
        NEXT();
        return 1;
    } else if (CURTOKt == T_STR) {
        tword *p = utf8_decode((unsigned char *)CURTOK->data.str);
        int i, listtmp = genlocal();
        NEXT();
        VAL(V_IMMED, 0);
        i_storelocal(listtmp);
        for (i = 0; i < tvsize(p); ++i) {
            VAL(V_IMMED, (unsigned long long)C.externaddrs[pushidx]);
            i_addrlocal(listtmp, 1);
            VAL(V_IMMED, p[i]);
            i_call(2);
            tvpop(C.vst);
        }
        i_addrlocal(listtmp, 0);
        return 1;
    } else if (CURTOKt == '@' || CURTOKt == T_IDENT) {
        int i, isaddrof = 0;
        if (CURTOKt == '@') {
            isaddrof = 1;
            NEXT();
        }
        if ((i = tvindexof(NC.paramnames, CURTOK->data.str)) != -1) i_addrparam(i, isaddrof);
        else if ((i = tvindexof(C.externnames, CURTOK->data.str)) != -1) VAL(V_IMMED, (unsigned long long)C.externaddrs[i]);
        else if (funcidx(CURTOK->data.str) != -1) VAL(V_FUNC, funcidx(CURTOK->data.str));
        else {
            if (!tvcontains(C.locals, CURTOK->data.str)) tvpush(C.locals, CURTOK->data.str);
            i = tvindexof(C.locals, CURTOK->data.str);
            i_addrlocal(i, isaddrof);
        }
        NEXT();
        return 1;
    }
    return 0;
}

static int arglist() {
    int count = or_test();
    while (count && CURTOKt == ',') {
        SKIP(',');
        count += or_test();
    }
    return count;
}

static char** parameters() {
    char **ret = 0;
    while (CURTOKt == T_IDENT) {
        tvpush(ret, CURTOK->data.str);
        NEXT();
        if (CURTOKt != ',') break;
        SKIP(',');
    }
    return ret;
}
/* used in conjunction with `parameters' to build a varargs list */
static int varargs(char ***argnames) {
    if (CURTOKt == '*') NEXT();
    else return 0;
    if (CURTOKt != T_IDENT) error("expecting name of varargs");
    tvpush(*argnames, CURTOK->data.str);
    NEXT();
    return 1;
}

static int trailer() {
    int i;
    if (CURTOKt == '(') {
        int rv = genlocal();
        NEXT();
        i = arglist();
        SKIP(')');
        i_call(i);
        i_storelocal(rv);   /* store rv to local in case of e.g. return a() + b() */
        i_addrlocal(rv, 0);
        return 1;
    } else if (CURTOKt == '[') {
        NEXT();
        i = or_test();
        if (i == 0) error("expecting subscript");
        SKIP(']');
        i_subscript();
        return 1;
    } else if (CURTOKt == '.') {
        NEXT();
        if ((i = tvindexof(C.accessorNames, CURTOK->data.str)) != -1) {
            SKIP(T_IDENT);
            VAL(V_IMMED, C.accessorOffsets[i]);
            i_subscript();
            return 1;
        } else error("no accessor named '%s'", CURTOK->data.str);
    }
    return 0;
}

static int atomplus() {
    int ret = atom();
    while (trailer()) {}
    return ret;
}

static int factor() {
    if (CURTOKt == '+' || CURTOKt == '-' || CURTOKt == '~') {
        int op = CURTOKt, count;
        NEXT();
        count = factor();
        i_mathunary(op);
        return count;
    }
    else return atomplus();
}

#define EXPRP(name, sub, tok0, tok1, tok2)                              \
static int name() {                                                     \
    int count = sub();                                                  \
    while (CURTOKt == tok0 || CURTOKt == tok1 || CURTOKt == tok2) {     \
        int op = CURTOKt;                                               \
        NEXT();                                                         \
        count += sub();                                                 \
        i_math(op);                                                     \
    }                                                                   \
    return count > 0;                                                   \
}
EXPRP(term, factor, '*', '/', '%')
EXPRP(arith_expr, term, '+', '-', '-')
EXPRP(and_expr, arith_expr, '&', '&', '&')
EXPRP(xor_expr, and_expr, '^', '^', '^')
EXPRP(expr, xor_expr, '|', '|', '|')

static int comparison() {
    char cmps[] = { '<', '>', KW(<=), KW(>=), KW(==), KW(!=) };
    int count = expr();
    for (;;) {
        Token* cmp = CURTOK;
        if (!tvcontainsn_nonnull(cmps, CURTOKt, 6)) break;
        NEXT();
        count += expr();
        i_cmp(cmp->type);
    }
    return count;
}

static int not_test() {
    if (CURTOKt == KW(not)) {
        SKIP(KW(not));
        comparison();
        VAL(V_IMMED, 0);
        i_cmp(KW(==));
        return 1;
    }
    else return comparison();
}

#define BOOLOP(name, sub, kw, cond)             \
static int name() {                             \
    char *label = 0;                            \
    int count = sub();                          \
    if (count && CURTOKt == KW(kw)) {           \
        int tmp = genlocal(), done = 0;         \
        for (;;) {                              \
            i_storelocal(tmp);                  \
            i_addrlocal(tmp, 0);                \
            label = i_jmpc(cond, label);        \
            if (done) break;                    \
            SKIP(KW(kw));                       \
            count += sub();                     \
            done = CURTOKt != KW(kw);           \
        }                                       \
        i_addrlocal(tmp, 0);                    \
    }                                           \
    i_label(label);                             \
    return count;                               \
}
BOOLOP(and_test, not_test, and, 0)
BOOLOP(or_test, and_test, or, 1)

static void expr_stmt() {
    or_test();
    if (CURTOKt == '=') {
        NEXT();
        or_test();
        i_store();
    }
    else tvpop(C.vst); /* discard */
    SKIP(T_NL);
}

static void stmt(int top) {
    char *labeldone = 0, *labeltest = 0, **argnames, *name;
    if (CURTOKt == KW(return)) {
        SKIP(KW(return));
        if (CURTOKt == T_NL || CURTOKt == T_DEDENT) VAL(V_IMMED, 20710);
        else or_test();
        i_ret();
        SKIP(T_NL);
    } else if (CURTOKt == KW(print)) {
        SKIP(KW(print));
        NEXT();
        SKIP(T_NL);
    } else if (CURTOKt == KW(pass)) {
        NEXT();
        SKIP(T_NL);
    } else if (CURTOKt == KW(for)) {
        char *lenstr = strintern("len"), *topofloop;
        int iterpos = genlocal(), iterthru = genlocal(), lenidx = tvindexof(C.externnames, lenstr), itervar;
        NEXT();
        VAL(V_IMMED, 0);
        i_storelocal(iterpos); /* store 0 into the indexer */
        if (!tvcontains(C.locals, CURTOK->data.str)) tvpush(C.locals, CURTOK->data.str);
        itervar = tvindexof(C.locals, CURTOK->data.str); /* this is the value in the list */
        NEXT();
        SKIP(KW(in));
        or_test(); /* this is the list */
        i_storelocal(iterthru);

        /* now we're set up. run through the list, incrementing iterpos, dereferencing the
           list into itervar, and exit when we get to the end */
        topofloop = C.codep;
        i_addrlocal(iterpos, 0);                                    /* stack: pos */
        VAL(V_IMMED, (unsigned long long)C.externaddrs[lenidx]);    /* stack: pos, lenfunc */
        i_addrlocal(iterthru, 0);                                   /* stack: pos, lenfunc, L */
        i_call(1);                                                  /* stack: pos, length */
        i_cmp('<');
        labeltest = i_jmpc(0, 0);  /* if not less, we're done */

        /* itervar = L[iterpos] */
        i_addrlocal(iterthru, 0);
        i_addrlocal(iterpos, 0);
        i_subscript();
        i_storelocal(itervar);

        /* finally get around to doing body of loop */
        suite();

        /* increment */
        i_addrlocal(iterpos, 0);
        VAL(V_IMMED, 1);
        i_math('+');
        i_storelocal(iterpos);

        /* jump to top of loop at comparison */
        i_jmpc(J_UNCOND, (char*)(topofloop - C.codep + C.codeseg - 5)); /* abuse i_jmpc for unconditional neg branch (5 is this instr size) */
        i_label(labeltest);
    } else if (CURTOKt == KW(if)) {
        SKIP(KW(if));
        or_test();

        labeltest = i_jmpc(0, 0);
        suite();
        labeldone = i_jmpc(J_UNCOND, 0);
        i_label(labeltest);

        while (CURTOKt == KW(elif) || CURTOKt == KW(else)) {
            NEXT();
            if (PREVTOK.type == KW(elif)) {
                or_test();
                labeltest = i_jmpc(0, 0);
            } else labeltest = 0;
            suite();
            if (labeltest) {
                labeldone = i_jmpc(J_UNCOND, labeldone);
                i_label(labeltest);
            }
        }
        i_label(labeldone);
    } else if (top && CURTOKt == KW(extern)) {
        SKIP(KW(extern));
        i_extern(CURTOK->data.str);
        NEXT();
        SKIP(T_NL);
    } else if (top && CURTOKt == KW(class)) {
        char *listunshift = strintern("listaddr_unshift"), **fakeargs = 0, *argsstr = strintern("args"), *pname = 0;
        int i, unshiftidx = tvindexof(C.externnames, listunshift);
        SKIP(KW(class));
        name = CURTOK->data.str;
        SKIP(T_IDENT);
        SKIP(':');
        argnames = parameters();
        for (i = 0; i < tvsize(argnames); ++i) addAccessor(name, argnames[i], i + 1);
        /* hmm, sort of smells like this should entirely be a simple macro, once there's macros */
        /* ctor for struct general idea is to make these functions:
            def <ctorname>(*args):
                listaddr_unshift(@args, <ctorname>)
                return args
        */
        tvpush(fakeargs, argsstr);
        i_func(name, fakeargs, 1);

        /* unshift ctor index onto args */
        VAL(V_IMMED, (unsigned long long)C.externaddrs[unshiftidx]);
        i_addrparam(0, 1);
        VAL(V_IMMED, tvindexof(C.funcnames, name));
        i_call(2);
        /* unshift returns list, we return that */
        i_ret();
        i_endfunc();

        /* predicate function for testing type equality. equiv to
            def <ctorname>?(obj):
                return obj[0] == <ctorname>
        */
        for (i = 0; i < tvsize(name) - 1; ++i) tvpush(pname, name[i]);
        tvpush(pname, '?'); tvpush(pname, '\0');
        pname = strintern(pname);
        i_func(pname, fakeargs, 0);
        i_addrparam(0, 0);
        VAL(V_IMMED, 0);
        i_subscript();
        VAL(V_IMMED, tvindexof(C.funcnames, name));
        i_cmp(KW(==));
        i_ret();
        i_endfunc();
    } else if (top && CURTOKt == KW(def)) {
        int hasvarargs;
        SKIP(KW(def));
        C.locals = NULL;
        name = CURTOK->data.str;
        SKIP(T_IDENT);
        SKIP('(');
        argnames = parameters();
        hasvarargs = varargs(&argnames);
        i_func(name, argnames, hasvarargs);
        SKIP(')');
        suite();
        i_endfunc();
    } else if (CURTOKt == T_NL) error("bad indent");
    else expr_stmt();
}

static void suite() {
    SKIP(':');
    SKIP(T_NL);
    SKIP(T_INDENT);
    stmt(0);
    while (CURTOKt != T_DEDENT)
        stmt(0);
    SKIP(T_DEDENT);
}

static void fileinput() {
    while (CURTOKt != T_END) {
        if (CURTOKt == T_NL) NEXT();
        else stmt(1);
    }
    SKIP(T_END);
}

/*
 * builtin functions
 */

static tword* tlistPush(tword** L, tword i) { tvpush(*L, i); return *L; }
static void tlistPop(tword* L) { tvpop(L); }
static int tlistLen(tword* L) { return tvsize(L); }
static tword *tRange(tword upper) {
    int i;
    tword *ret = NULL;
    for (i = 0; i < upper; ++i) tvpush(ret, i);
    return ret;
}
static tword tlistShift(tword* L) {
    tword ret = L[0];
    int i;
    for (i = 0; i < tvsize(L) - 1; ++i) L[i] = L[i + 1];
    tvpop(L);
    return ret;
}
static tword* tlistUnshift(tword **L, tword v) {
    int i;
    tvpush(*L, -1);
    for (i = tvsize(*L) - 2; i >= 0; --i) (*L)[i + 1] = (*L)[i];
    (*L)[0] = v;
    return *L;
}
static void tlistReverse(tword *L) {
    int i;
    for (i = 0; i < tvsize(L) / 2; ++i) {
        tword tmp = L[i];
        L[i] = L[tvsize(L) - 1 - i];
        L[tvsize(L) - 1 - i] = tmp;
    }
}

typedef struct NamePtrPair { char *name; void *func; } NamePtrPair;
static NamePtrPair stdlibFuncs[] = {
    { "len", tlistLen },
    { "listaddr_push", tlistPush },
    { "listaddr_unshift", tlistUnshift },
    { "mempop", tba_popcheckpoint },
    { "mempush", tba_pushcheckpoint },
    { "pop", tlistPop },
    { "range", tRange },
    { "reverse", tlistReverse },
    { "shift", tlistShift },
};
static int strcmpPair(const void *a, const void *b) { return strcmp(((NamePtrPair*)a)->name, ((NamePtrPair*)b)->name); }
static void *stdlibLookup(char *name) {
    NamePtrPair tmp = { name, 0 };
    NamePtrPair *p = bsearch(&tmp, stdlibFuncs, tarrsize(stdlibFuncs), sizeof(NamePtrPair), strcmpPair);
    return p ? p->func : NULL;
}
static void exportStdlib() {
    int i;
    for (i = 0; i < tarrsize(stdlibFuncs); ++i) i_extern(stdlibFuncs[i].name);
}

/*
 * main api entry points
 */
void twokHttpRepl(void *(*externLookup)(char *name)) {
    int sockfd, newfd, rv, yes=1, numbytes;
    struct addrinfo hints, *servinfo;
    struct sockaddr_storage theirAddr; /* connector's address information */
    (void)(externLookup);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; /* use my IP */
    if ((rv = getaddrinfo(NULL, "20710", &hints, &servinfo)) != 0) error("getaddrinfo");
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) error("socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) error("setsockopt");
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) error("bind");
    freeaddrinfo(servinfo);
    if (listen(sockfd, 2) == -1) error("listen");
    printf("Do:\n    curl -d \"code\" localhost:20710/\nor:\n    curl -d @filename localhost:20710/\nto eval code. Shutdown with:\n    curl -d \"\" localhost:20710/quit\n");
    for (;;) {
        socklen_t sinsize = sizeof(theirAddr);
        char buf[32768], *p;
        newfd = accept(sockfd, (struct sockaddr *)&theirAddr, &sinsize);
        if (newfd == -1) error("accept");
        if ((numbytes = recv(newfd, buf, sizeof(buf)-1, 0)) == -1) error("recv");
        buf[numbytes] = 0;
        p = strstr(buf, "\r\n\r\n");
        if (!p) error("bad input");
        p += 4;
        if (strncmp(&buf[5], "/ ", 2) == 0) {
            /* hmm, need global scope code to run non-__main__ funcs */
        } else if (strncmp(&buf[5], "/quit ", 6) == 0) {
            if (send(newfd, "Shutting down.\r\n", 16, 0) == -1) error("send");
            break;
        }
        close(newfd);
    }
}

int twokRun(char *code, void *(*externLookup)(char *name)) {
    int ret, allocSize, entryidx;
    memset(&C, 0, sizeof(C));
    C.input = code;
    C.externLookup = externLookup;
    allocSize = 1<<17;
    if (setjmp(C.errBuf) == 0) {
        tba_pushcheckpoint();
        exportStdlib();
        tokenize();
        /* dump tokens generated from stream */
#if 0
        { int j;
        for (j = 0; j < tvsize(C.tokens); ++j) {
            if (C.tokens[j].type == T_NUM)
                printf("%3d: %3d %lld\n", j, C.tokens[j].type, C.tokens[j].data.tokn);
            else
                printf("%3d: %3d %s\n", j, C.tokens[j].type, C.tokens[j].data.str);
        }}
#endif
        C.codeseg = C.codep = twok_allocExec(allocSize);
        C.codesegend = C.codeseg + allocSize;
        i_setup();
        fileinput();
        if (tvsize(C.vst) != 0) error("internal error, values left on stack");
        /* dump disassembly of generated code, needs ndisasm in path */
#if 0
        { FILE* f = fopen("dump.dat", "wb");
        //fwrite(C.codeseg + 64*NC.varargsgetsize, 1, C.codep - C.codeseg - 64*NC.varargsgetsize, f);
        fwrite(C.codeseg, 1, C.codep - C.codeseg, f);
        fclose(f);
        ret = system("ndisasm -b64 dump.dat"); }
#endif
#if 0
        { FILE* f = fopen("dump.o", "wb");
        fwrite(NC.symfile, 1, tvsize(NC.symfile), f);
        fclose(f); }
#endif
        entryidx = funcidx("__main__");
        if (entryidx == -1) error("no entry point '__main__'");
        tba_popcheckpoint();
        ret = ((int (*)())(C.codesegend - (entryidx + 1) * FUNC_THUNK_SIZE))();
    } else {
        tba_popcheckpoint();
        ret = -1;
    }
    twok_freeExec(C.codeseg, allocSize);
    return ret;
}

#endif /* TWOK_DEFINE_IMPLEMENTATION */


