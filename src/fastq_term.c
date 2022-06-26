/*
Initialize the window for printing the fastq data, additionally process
where to but the characters.

General notes:
    - The buffer on the screen will be serially written into then updated, fortunately ncurses controls
    the baud rate of the screen to help me out!
    - Each string will need to belong a certain column which must always have atleast one space before the next
    sequence

First Idea:
    1.  Create a hashmap one position for each column in the terminal.
    2. initialize each column with either a fastq value or a cool down period 
        - The cool down period determining how long until a new read can be added to that position
    
    It may be easier to create an array of pointer with each row pointing to a row on the terminal,
    so when a line advances, memory is copied to the next row filling the previous one.

    So each columns gets picked, nucleotides are consumed one by one filling rows.

    the struct the hashmap refers too should contain the fastq_nucleotides, and a cool down parameter
    e.g.
     struct display{
         fastq_nucleotides*;
         uint length_of_string;
         uint cooldown;
         uint position; The position to set the array too
     } Whenever a new set of fastq_nucleoties is to be loaded in the cooldown can be reset!


     Need a set to decide on if a position is or isnt in use. Might keep a set of positions, and a queue
     loaded up of new positions, with a position in the set. And with cool down positions taken up,
     when a col is 0 it is free and a cool down can be added

     set_watch = [pos1, pos2, pos3, pos4, pos5, pos6 ... posN]
     Values in the array:

     set_watch = [0, 0, 0, 0, 0, 0 ... 0]
     set_watch = [10, 5, 8, 8, 2 ,4 ...]


    hashmap of column positions:
                            pos_1 = fastq_nucleotides
                            pos_2 = fastq_nucleotides
                            pos_3 = fastq_nucleotides
                            pos_4 = fastq_nucleotides
                            pos_5 = fastq_nucleotides
                            pos_6 = fastq_nucleotides
                            pos_7 = fastq_nucleotides
                            pos_8 = fastq_nucleotides  
                            pos_9 = fastq_nucleotides

    each column maps to a row, with rows matching the number available on the terminal

            rows = [
                terminal_row* = [ -------------------------------------------------- ]
                terminal_row* = [ -------------------------------------------------- ]
                terminal_row* = [ -------------------------------------------------- ]
            ]

    the rows can then be transcribed into the screens buffer


Second Idea:
    I have realized attempt 1 is likely overly complicated. It would likely be better to create a 
    coordinate system with some defines progress values through their coordinates.

   e.g. load a character up, and progress it along the screen. every character in a buffer needs to be incremented one
    row length till off the screen 

Third Idea:
    Treat each column as an object, which holds the entirety of the fastq string. So the whole array can be populated and different values can be incremented
    
    So create an array of structs (kind of as suggested earlier), and initialize them with a char array the size of the row lengths. 
    The struct can be defined as:
        struct term_col{
            uint8_t column_idx; // the column in which to populate the array
            fastq_nucleotides* nuc_chars; // The array carrying the characters
            fastq_nucleotides* column; // The column to fill  this actualy needs to be a struct array to hold quality tag
        }
    
    Then the array of columns a can be moved through iteratively updating the columns and populating the screen buffer

    Functions needed:
        1. One to create the structs for each terminal column
        2. One to increment the character in the column array by one each time, then load a new nuc into the array.
           clearing the final row each time.
        3. One to randomly assign a sequence to a column

TODO: Need to make the windows size struct accessible everywhere, and constant so that if 
window is resized while running it does not result in a segfault



2022-06-20: Matthew Wells
*/

#include <sys/ioctl.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <ncurses.h>
#include <string.h>
#include "parse_fastq.h"

#define TERM_SIZE(x, y) (x * y)
#define FILL_CHAR ' '
#define LINE_SIZE 256 // may need to track row length in the structs

typedef struct terminal_col{
    uint8_t column_idx; // to hold column position
    fastq_nucleotide* nucleotide_characters; // array of quality data
    uint32_t line_length;
    fastq_nucleotide* column; // the column row to fill, will be length of rows available
    uint8_t cooldown; // Randomly assign a cool down to a struct to start displaying data
    bool col_used; // If the column is empty or not
}terminal_col;

terminal_col* terminal_fastq_data(struct winsize ws, uint32_t fq_nuc_counter){
    /*
        Function: terminal_fastq_data
        -----------------------------
        Create a set of structs to hold the data to be sent to the terminal for display 

        TODO: the terminal display data struct needs to be kept close by, so need to find out
        if it can be statically allocated

        winsize: The winsize struct holding terminal struct sizes
        fq_nuc_counter: The counter passed from the fastq_nucleotides struct, can be used as a progress bar and to know when out of fastq's
        return: A pointer to an array holding the struct terminal col
    */

   static uint32_t _FASTQ_COUNTER_ = fq_nuc_counter;
   terminal_col* display_data = malloc(ws.ws_row * sizeof(*display_data));
   for(size_t i = 0; i < ws.ws_row; i++){
       display_data[i].column_idx = i;
       display_data[i].column = NULL;
       display_data[i].nucleotide_characters = NULL;
       display_data[i].line_length = 0;
       display_data[i].col_used = false;
       
   }
   return display_data;
}

void destroy_term_fq(terminal_col* terminal_fq_data, struct winsize ws){
    /*
        Function: destroy_term_fq
        -------------------------
        Destroy the terminal_col struct  

        terminal_fq_data: The columns in the terminal displaying data to the screen
        ws: The window size struct to get row lengths
        return: void 
    */
   for(size_t i = 0; i < ws.ws_row; i++){
       free(terminal_fq_data[i].column);
       free(terminal_fq_data[i].nucleotide_characters);
       free(&terminal_fq_data[i]);
   }
}

void load_fastq_terminal(terminal_col** terminal_data, fastq_nucleotides** fq_data, struct winsize ws){
    /*
        Function: load_fastq_terminal
        -----------------------------
        Initialize the terminal data with the initial set of data. Need to make sure
        the array is not written past as well. 

        Future Implementation Note:
        ---------------------------
        This needs to be called every cycle to reload the buffer, can make a check to
        randomly assign values back to them if theyre not filled.

        terminal_data: The columns to be displayed to the screen for the data
        fq_data: The fastq data to be presented to the screen
        ws: The win struct size
        return: void
     */


    uint8_t buffer_counter = ws.ws_row;
    while(buffer_counter != 1 && (*fq_data)->counter != 0){
        // need to beef up these gaurd conditions
        if(!(*terminal_data)->col_used){
            // not sure if this is the correct way to add an additonal pointer to some memory
            (*terminal_data)->nucleotide_characters = (*fq_data)->data[(*fq_data)->counter];
            // magic number of 10 is just for testing cooldown
            // In reality  it may be better to set rand max and have rand() % rand() 
            // to make cooldown more random
            (*terminal_data)->cooldown = rand() % 10;
            (*terminal_data)->line_length = LINE_SIZE;
            (*terminal_data)->col_used = true;
            (*fq_data)->counter--;
        }
    }

}

void progress_terminal(terminal_col** term_data, struct winsize ws){
    /*
        Function: progress_terminal
        ---------------------------
        Progress characters in terminal buffer from the nucleotide array into  the column

        term_data: Struct of terminal_col 
        ws: The windows size struct for column length
        return: void
    */

   for(size_t i = 0; i < ws.ws_col; i++){
       terminal_col* t_data = &(*term_data)[i];
       // left off here
   }
}


struct winsize get_window_size(){
    /*
        Function: get_windows_size
        --------------------------
        Return the winsize struct which contains the values required for determining the 
        terminals size.

        return: struct winsize 
    
    */
    struct winsize ws; //winsize holds two unsigned shorts, according to termios.h
    int fd;
    fd = open("/dev/tty", O_RDWR);
    if(fd < 0){
        printf("Could not find terminal in /dev/tty \n");
        err(1, "/dev/tty");
    }
    if(ioctl(fd, TIOCGWINSZ, &ws) < 0){
        printf("An error with ioctl that I did not google\n");
        err(1, "/dev/tty");
    }

    close(fd);
    return ws;
}


char* get_term_window(struct winsize window){
    /*
    Need to figure out how to get attribute inline to work properly and toggle on or off

        Function: get_term_window
        -------------------------

        Create the terminal window buffer and set the memory to one static value for test purposes

        window: A winsize struct to get termninal bounds

        return: char pointer to the buffer in the terminal 
    */

   // get a warning from c++, not an issue in C
    char* term_window = malloc((window.ws_col * window.ws_row) * sizeof(*term_window));
    memset(term_window, FILL_CHAR, TERM_SIZE(window.ws_col, window.ws_row)); // upgrade to memset_s for safety checks
    return term_window;
}


uint32_t row_rand_position(unsigned short row_length){
    /*
        Function: row_rand_position
        ---------------------------
        Return some random position in the row to populate with a character.
        
        row_length: The struct winsize ws_row attribute
        return: Row position 
    */
   uint32_t r = rand() % row_length;
   return r;
}


void test_populate_rows(char** term_buffer, unsigned short row_length){
    /*
        Function: test_populate_rows
        -----------------------
        Put some random characters into the first row of the terminal buffer

        term_buffer: The array to be printed to the terminal
        row_length: The row lenght to be populated
        return: nothing
    */
   uint32_t number_rows_fill = row_length * 0.1; // populate 50% of the columns with chars
   for(size_t i = 0; i < number_rows_fill; i++){
       uint32_t val = row_rand_position(row_length);
       (*term_buffer)[val] = 'X'; // fill the buffer with some arbitrary value
   }
   
}


void test_increment_vals(char** term, const struct winsize* ws_, const unsigned long int row_val_){
    /*
        Function: test_increment_vals
        ------------------------
        Increment the where the x is, moving it down the screen

        term: A pointer to the start of the array
        ws_: the winsize struct holding the array bounds Note: had this as restrict but when liniting got a c++ error not an issue in the future
        row_val: which row to place the new information
        return: void 
    */

    size_t term_size = TERM_SIZE(ws_->ws_col, ws_->ws_row);
    unsigned short row_val = row_val_ % ws_->ws_row;

    // Clear the bottome row out each time 
    for(size_t g = ((ws_->ws_row * ws_->ws_col) - ws_->ws_col); g < term_size; g++){
        (*term)[g] = FILL_CHAR;
    }
    for(size_t i = term_size; i != -1; i--){
        if((*term)[i] == 'X'){
            (*term)[i + ws_->ws_col] = 'X';
            (*term)[i] = FILL_CHAR; // bounds check not needed
        }
    }
}





int main(){
    struct winsize ws = get_window_size();
    size_t term_size = TERM_SIZE(ws.ws_col, ws.ws_row); // should make this static so does not always need to be recalculated
    char* term = get_term_window(ws);
    test_populate_rows(&term, ws.ws_col); // passing col, as that is the number of rows
    fprintf(stderr, "Rows size %ud", ws.ws_row);
    initscr();
    mvprintw(0, 0, term);
    refresh();
    unsigned long int accu = 1; // skip first row, so row position is always positive
    while (1)
    {
        if((accu % 2) == 0){
            test_populate_rows(&term, ws.ws_col);
        }
        test_increment_vals(&term, &ws, accu);
        mvprintw(0, 0, term);
        refresh();
        ++accu;
    }
    
    endwin();
    free(term);
    return 0;
}



