// testing out ncurses
#include <ncurses.h>
#include <stdio.h>

int main(){
    initscr();
    mvprintw(0,0, '\0'); // adding null byte, just want to set position
    printw("Hello world!");
    refresh();
    getch();
    endwin();

    return 0;
}