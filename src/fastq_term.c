/*

Initialize the window for printing the fastq data, additionally process
where to but the characters.




General notes:
    - The buffer on the screen will be serially written into then updated, fortunately ncurses controls
    the baud rate of the screen to help me out!
    - Each string will need to belong a certain column which must always have atleast one space before the next
    sequence

First attempt:
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


Attempt 2:
    I have realized attempt 1 is likely overly complicated. It would likely be better to create a 
    coordinate system with some defines progress values through their coordinates.

   e.g. load a character up, and progress it along the screen. every character in a buffer needs to be incremented one
    row length till off the screen 


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

#define TERM_SIZE(x, y) (x * y)

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
    char* term_window = malloc((window.ws_col * window.ws_row) * sizeof(term_window));
    memset(term_window, ' ', TERM_SIZE(window.ws_col, window.ws_row));
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


void populate_rows(char** term_buffer, unsigned short row_length){
    /*
        Function: populate_rows
        -----------------------
        Put some random characters into the first row of the terminal buffer

        term_buffer: The array to be printed to the terminal
        row_length: The row lenght to be populated
        return: nothin 
    */
   uint32_t number_rows_fill = row_length * 0.75; // populate 50% of the columns with chars
   for(size_t i = 0; i < number_rows_fill; i++){
       uint32_t val = row_rand_position(row_length);
       (*term_buffer)[val] = 'X'; // fill the buffer with some arbitrary value
   }
   
}


void increment_vals(char** term, struct winsize ws_, unsigned short row_val){
    for(size_t i = 0; i < (ws_.ws_col * row_val); i++){
        if((*term)[i] == 'X'){
            (*term)[i + ws_.ws_col] = 'X';
        }
    }
}


int main(){
    struct winsize ws = get_window_size();
    size_t term_size = TERM_SIZE(ws.ws_col, ws.ws_row);
    char* term = get_term_window(ws);
    populate_rows(&term, ws.ws_col); // passing col, as that is the number of rows
    fprintf(stderr, "Rows size %ud", ws.ws_row);
    initscr();
    mvprintw(0,0, term);
    refresh();
    getch();
    unsigned short accu = 0;
    while (accu != ws.ws_row)
    {
        increment_vals(&term, ws, accu);
        mvprintw(0, 0, term);
        accu++;
        getch();
    }
    
    endwin();
    term[term_size] = '\0';
    return 0;
}




