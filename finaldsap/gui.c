#include"buffer.h"
#include"gui.h"
#include<ncurses.h>

// Function to initialize window
void init_window() {
    initscr();  /* Start curses mode */
    raw(); //most programs require that the characters be available as soon as the user types them
    curs_set(2); //Terminal-specific high visibility mode
    start_color();
    /* Initialize all the colors */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
}

// Function to display contents of buffer on window
void loadwin(buffer *bf, int y) {
    clear();
    refresh();
    attron(COLOR_PAIR(3)); //attron just switches on the attribute given to it
    int x = 0;
    while(bf != NULL) {
        mvprintw(y,x, "%s", bf->line);
        y++;
        bf = bf->next;
    }
    refresh();
}