#include "window_size.h"


int rand_char();
void fill_print_buffer(char*, struct winsize);


int main(){
    struct winsize ws = get_window_size();
    char* term_window = malloc((ws.ws_col * ws.ws_row) * sizeof(term_window));

    fill_print_buffer(term_window, ws);
    term_window[ws.ws_col * ws.ws_row] = '\0';
    
    fill_print_buffer(term_window, ws);
    term_window[ws.ws_col * ws.ws_row] = '\0';
    printf("%s\n", term_window);
    
    fill_print_buffer(term_window, ws);
    term_window[ws.ws_col * ws.ws_row] = '\0';
    printf("%s\n", term_window);
    
    fill_print_buffer(term_window, ws);
    term_window[ws.ws_col * ws.ws_row] = '\0';
    printf("%s\n", term_window);
    
    fill_print_buffer(term_window, ws); 
    term_window[ws.ws_col * ws.ws_row] = '\0';
    printf("%s\n", term_window);
   
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
