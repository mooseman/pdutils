/* slinit.c - Initialize SLsmg layer, and shut it down */

#include <slang/slang.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

int main(void) {
    struct winsize ws;

    /* get the size of the terminal connected to stdout */
    if (ioctl(1, TIOCGWINSZ, &ws)) {
        perror("failed to get window size");
        return 1;
    }

    SLtt_get_terminfo();

    SLtt_Screen_Rows = ws.ws_row;
    SLtt_Screen_Cols = ws.ws_col;

    SLsmg_init_smg();

    /* heart of the program goes here */

    SLsmg_gotorc(SLtt_Screen_Rows - 1, 0);
    SLsmg_refresh();
    SLsmg_reset_smg();
    SLang_reset_tty();

    return 0;
}
