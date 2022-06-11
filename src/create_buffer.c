#include "window_size.h"
#include <ncurses.h>
#include <time.h>
#include <string.h>

int rand_char();
void fill_print_buffer(char*, struct winsize);


int main(){
    srand(time(0));
    struct winsize ws = get_window_size();
    char* term_window = malloc((ws.ws_col * ws.ws_row) * sizeof(term_window));

    fill_print_buffer(term_window, ws);

    initscr(); // initialize screen
    mvprintw(0, 0, term_window); // mv cursor to start and prin screen
    fill_print_buffer(term_window, ws); // randomly populate the buffer
    getch(); // wait for key press
    refresh(); //refresh contents
    
    mvprintw(0, 0, term_window); // move back to start and print screen
   // clear();
    //printw(term_window);
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


int rand_char(){
    int r = rand(); 
    int ret_char = r % (26);
    return ret_char + 'A'; // offset the alphabet to starting position
}
