/* slecho.c - Simple demonstration of SLANG input handling */
#include <ctype.h>
#include <slang/slang.h>
#include <stdio.h>

int main(void) {
    char ch;

    /*
       Start SLANG tty handling, with:
        -1 default interrupt character (normally, Ctrl-C)
         0 no flow control, allowing all characters (except interrupt) to
           pass through to the program
         1 enable OPOST output processing of escape sequences
    */
    SLang_init_tty(-1, 0, 1);

    while (ch != 'q' && SLang_input_pending(20)) {
        ch = SLang_getkey();
        printf("read: %c 0x%x\n", isprint(ch) ? ch : ' ', ch);
    }

    SLang_reset_tty();

    return 0;
}

