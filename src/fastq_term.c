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
         uint cooldown;

     } Whenever a new set of fastq_nucleoties is to be loaded in the cooldown can be reset!

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


2022-06-20: Matthew Wells
*/

#include <sys/ioctl.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define TERM_SIZE(x, y) (x * y)

struct winsize get_window_size(){
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





