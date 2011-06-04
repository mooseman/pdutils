/* slcharset.c - S-Lang program to show normal and alternate 
   character sets */
#include <slang/slang.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

/* displays a table containing 256 characters in a single character 
   set, starting a column col.  The 'label' is displayed over the
   table, and the alternate character set is displayed iff
   isAlternate is nonzero */
static void drawCharSet(int col, int isAlternate, char * label) {
    int i, j;
    int n = 0;

    /* draw the box */
    SLsmg_draw_box(0, col, 20, 38);

    /* center the label */
    SLsmg_gotorc(0, col + 2);
    SLsmg_write_string(label);


    /* draw the horizontal legend */
    SLsmg_gotorc(2, col + 4);
    SLsmg_write_string("0 1 2 3 4 5 6 7 8 9 A B C D E F");

    /* set the character set to use */
    SLsmg_set_char_set(isAlternate);

    /* this iterates over the 4 most significant bits */
    for (i = 0; i < 16; i++) {
        SLsmg_gotorc(3 + i, 2 + col);
        SLsmg_write_char(i < 10 ? i + '0' : (i - 10) + 'A');

	/* this iterates over the 4 least significant bits */
        for (j = 0; j < 16; j++) {
            SLsmg_gotorc(3 + i, col + 4 + (j * 2));
            SLsmg_write_char(n++);
        }
    }

    SLsmg_set_char_set(0);
}

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
    SLang_init_tty(-1, 0, 1);

    drawCharSet(0, 0, "Normal Character Set");
    drawCharSet(40, 1, "Alternate Character Set");

    SLsmg_refresh();
    SLang_getkey();

    SLsmg_gotorc(SLtt_Screen_Rows - 1, 0);
    SLsmg_refresh();
    SLsmg_reset_smg();
    SLang_reset_tty();

    return 0;
}
