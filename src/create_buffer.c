#include "window_size.h"
#include <ncurses.h>
#include <time.h>
#include <string.h>

int rand_char();
int rand_color_pair();
void fill_print_buffer(char*, struct winsize);



int main(){
    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    
    
    srand(time(0));
    struct winsize ws = get_window_size();
    char* term_window = malloc((ws.ws_col * ws.ws_row) * sizeof(term_window));

    fill_print_buffer(term_window, ws);

    // defining color pairs
    if(has_colors() == FALSE){
        printf("Terminal does not have color support exiting\n");
        endwin();
        exit(1);
    }

    mvprintw(0, 0, term_window); // mv cursor to start and prin screen
    fill_print_buffer(term_window, ws); // randomly populate the buffer
    getch(); // wait for key press
    refresh(); //refresh contents
    
    mvprintw(0, 0, term_window); // move back to start and print screen
   // clear();
    //printw(term_window);
    refresh();
    getch();
    clear();
    move(0,0); // move to position to add the char
    fill_print_buffer(term_window, ws);
    for(size_t i = 0; i < (ws.ws_col * ws.ws_row); ++i){
        //need to set a colour attribute eachtime something is written
        //attron(COLOR_PAIR(1));
        addch(term_window[i] | COLOR_PAIR(rand_color_pair()));
       // attroff(COLOR_PAIR(1)); // good idea to restore terminal state each time
    }
    

    refresh();
    getch();
    endwin();
   
    free(term_window);
}


void fill_print_buffer(char* char_buff, struct winsize ws){
    for(size_t i = 0; i < ws.ws_row; i++){
        for(size_t f = 0; f < ws.ws_col; f++){
            char_buff[(i * ws.ws_col) + f] = rand_char();
        }
        //term_window[i * ws.ws_col] = '\n';
    }
    char_buff[ws.ws_col * ws.ws_row] = '\0';
}

int rand_color_pair(){
    int r = rand() % 7;
    return r;

}

int rand_char(){
    int r = rand(); 
    int ret_char = r % 26;
    return ret_char + 'A'; // offset the alphabet to starting position
}
