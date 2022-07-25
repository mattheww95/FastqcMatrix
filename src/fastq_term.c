/*
Initialize the window for printing the fastq data, additionally process
where to but the characters.

General notes:
    - The buffer on the screen will be serially written into then updated,
fortunately ncurses controls the baud rate of the screen to help me out!
    - Each string will need to belong a certain column which must always have
atleast one space before the next sequence

First Idea:
    1.  Create a hashmap one position for each column in the terminal.
    2. initialize each column with either a fastq value or a cool down period
        - The cool down period determining how long until a new read can be
added to that position

    It may be easier to create an array of pointer with each row pointing to a
row on the terminal, so when a line advances, memory is copied to the next row
filling the previous one.

    So each columns gets picked, nucleotides are consumed one by one filling
rows.

    the struct the hashmap refers too should contain the fastq_nucleotides, and
a cool down parameter e.g. struct display{ fastq_nucleotides*; uint
length_of_string; uint cooldown; uint position; The position to set the array
too } Whenever a new set of fastq_nucleoties is to be loaded in the cooldown can
be reset!


     Need a set to decide on if a position is or isnt in use. Might keep a set
of positions, and a queue loaded up of new positions, with a position in the
set. And with cool down positions taken up, when a col is 0 it is free and a
cool down can be added

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

    each column maps to a row, with rows matching the number available on the
terminal

            rows = [
                terminal_row* = [
-------------------------------------------------- ] terminal_row* = [
-------------------------------------------------- ] terminal_row* = [
-------------------------------------------------- ]
            ]

    the rows can then be transcribed into the screens buffer


Second Idea:
    I have realized attempt 1 is likely overly complicated. It would likely be
better to create a coordinate system with some defines progress values through
their coordinates.

   e.g. load a character up, and progress it along the screen. every character
in a buffer needs to be incremented one row length till off the screen

Third Idea:
    Treat each column as an object, which holds the entirety of the fastq
string. So the whole array can be populated and different values can be
incremented

    So create an array of structs (kind of as suggested earlier), and initialize
them with a char array the size of the row lengths. The struct can be defined
as: struct term_col{ uint8_t column_idx; // the column in which to populate the
array fastq_nucleotides* nuc_chars; // The array carrying the characters
            fastq_nucleotides* column; // The column to fill  this actualy needs
to be a struct array to hold quality tag
        }

    Then the array of columns a can be moved through iteratively updating the
columns and populating the screen buffer

    Functions needed:
        1. One to create the structs for each terminal column
        2. One to increment the character in the column array by one each time,
then load a new nuc into the array. clearing the final row each time.
        3. One to randomly assign a sequence to a column

TODO: Need to make the windows size struct accessible everywhere, and constant
so that if window is resized while running it does not result in a segfault


Idea: Instead of storing a secondary array of nucleotide chars, make a window
sizzed array of nucleotide chars and can read that to screen repeatadly



2022-06-20: Matthew Wells
*/

#include "parse_fastq.h"
#include <err.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define TERM_SIZE(x, y) (x * y)
#define FILL_CHAR ' '
//#define TERM_FILL_CHAR fastq_nucleotide a = {.nucleotide = ' ', quality_value
//= ' ', color_pair = 0};
#define LINE_SIZE 256 // may need to track row length in the structs

/**
 * @brief The terminal column data type
 *
 */
typedef struct terminal_col {
  size_t column_idx;                       // to hold column position
  fastq_nucleotide *nucleotide_characters; // array of quality data
  uint32_t line_length;
  uint32_t array_pos; // Position within the nucleotide array to move to
                      // the buffer screen
  uint32_t line_idx;
  // fastq_nucleotide
  //     *column;      // the column row to fill, will be length of rows
  //     available
  uint8_t cooldown; // Randomly assign a cool down to a struct to start
                    // displaying data
  bool col_used;    // If the column is empty or not
} terminal_col;

static const fastq_nucleotide EMPTY = {
    .nucleotide = ' ', .quality_value = ' ', .color_pair = ' '};

/**
 * @brief Create a set of structs to hold the data to be sent to the
 * terminal display
 *
 * @param ws The winsize struct holding the terminal struct sizes
 * @param fq_nuc_counter  The counter passed from the fastq_nucleotides
 * struct, can be used a s progress bar in the future
 * @return terminal_col* The initialized display data
 */

terminal_col *terminal_fastq_data(struct winsize ws, uint32_t fq_nuc_counter) {

  static uint32_t _FASTQ_COUNTER_ = 0;
  _FASTQ_COUNTER_ = fq_nuc_counter;
  terminal_col *display_data = malloc(ws.ws_col * sizeof(*display_data));
  for (size_t i = 0; i < ws.ws_col; i++) {
    display_data[i].column_idx = i;
    // display_data[i].column = NULL;
    display_data[i].array_pos = 0;
    display_data[i].nucleotide_characters = NULL;
    display_data[i].line_length = 0;
    display_data[i].line_idx = 0;
    display_data[i].col_used = false;
  }
  return display_data;
}

/**
 * @brief Destroy the terminal_col struct
 *
 * @param terminal_fq_data The Columns in the terminal displaying the datat to
 * the screen
 * @param ws  The window size struct to get row lengths
 */
void destroy_term_fq(terminal_col *terminal_fq_data, struct winsize ws) {
  for (size_t i = 0; i < ws.ws_row; i++) {
    // free(terminal_fq_data[i].column);
    free(terminal_fq_data[i].nucleotide_characters);
  }
  free(terminal_fq_data);
}

/**
 * @brief Initialize the terminal data with the initial set of data. Need to add
 * bounds check on the array
 *
 * @param terminal_data The columns to be displayed to the screen for the data
 * @param fq_data  The fastq data to be presented to the screen
 * @param ws The win struct size
 */
void load_fastq_terminal(terminal_col **terminal_data,
                         fastq_nucleotides **fq_data, struct winsize ws) {
  /*
      TODO: Realized this is not initialting the structs properly e.g. not
     initializing all of them Future Implementation Note:
      ---------------------------
      This needs to be called every cycle to reload the buffer, can make a check
     to randomly assign values back to them if theyre not filled.
   */

  int16_t buffer_counter = ws.ws_col - 1;
  int16_t fq_cntr = (int16_t)(*fq_data)->counter--;
  while (buffer_counter != 0 && fq_cntr != 0) {
    // need to beef up these gaurd conditions
    if (!(*terminal_data)[buffer_counter].col_used) {

      (*terminal_data)[buffer_counter].nucleotide_characters =
          (*fq_data)->data[(*fq_data)->counter];

      // In reality  it may be better to set rand max and have rand() % rand()
      // to make cooldown more random
      (*terminal_data)[buffer_counter].cooldown = rand() % 10;
      (*terminal_data)[buffer_counter].line_length = LINE_SIZE;
      (*terminal_data)[buffer_counter].col_used = true;

      (*fq_data)->counter--;
      fq_cntr--;
    }
    buffer_counter--;
  }
}

/**
 *@brief Increment all characters in the terminal forward except
 *
 *@param window The display array of values to be incremented
 *@param ws Struct of winsize parameter
 * */
void increment_terminal(fastq_nucleotide *window, struct winsize ws) {
  // Need to increment the data forward while making sure not to move past
  // the end of the array or overwrite data

  size_t window_size = TERM_SIZE(ws.ws_col, ws.ws_row);

  for (size_t i = (ws.ws_col * ws.ws_row) - ws.ws_col; i < window_size; ++i) {
    window[i].nucleotide = EMPTY.nucleotide;
  }
  for (size_t f = window_size - 1; f != 0; --f) {
    if (window[f].nucleotide != EMPTY.nucleotide) {
      window[f + ws.ws_col] = window[f];
    }
  }
}

/**
 *@brief Display the array of fastq nucleotided data to the terminal
 *
 *@param window fastq data to be displayed to the terminal
 *@param ws Winsize struct
 *
 * */
void display_nucleotides(fastq_nucleotide *window, struct winsize ws) {

  for (size_t i = 0; i < TERM_SIZE(ws.ws_col, ws.ws_row) - 1; i++) {
    printf("%c", window[i].nucleotide);
  }
  printf("\n");
}

/**
 * @brief  Progress the characters in the terminal from the nucleotide array
 * into the column
 *
 * @param term_data Struct of the terminal_col type
 * @param ws The window size struct for column length
 * @param window The window array holding fastq nucleotide structs
 */

void progress_terminal(terminal_col **term_data, struct winsize ws,
                       fastq_nucleotide **window) {
  /*

      Steps for what this function must do, as i need to plan this:
          Step 1. Move through each of the terminal columns
          Step 2. If the column is in use, advance the characters in the
     terminal i. if it is not 0, add a character from the nuc chars into the
     column ii. advance the characters in the column forward 1 iii. decrement
     the column counter iv. Increment the line_idx value of the struct to get
     the next column iv. If the column is empty and/or cooldown is 0 set the
     column usage to false Step 3. If the usage is false load a new fastq
     seqeunce into the column
  */

  increment_terminal(*window, ws); // Increment all values in the terminal

  for (size_t i = 0; i < ws.ws_col; i++) {
    terminal_col *t_data = &(*term_data)[i];

    if (t_data->col_used) { // check if there are characters in the column

      // add a new nucleotided to the array window to display
      (*window)[t_data->column_idx] =
          t_data->nucleotide_characters[t_data->array_pos];

      // Test if at the end of the nucleotide yet
      if (t_data->line_length >= t_data->array_pos) {
        t_data->array_pos++;
      }
    }
  }
  printf("Window size %d\n", TERM_SIZE(ws.ws_col, ws.ws_row));
  display_nucleotides(*window, ws);
}

/**
 * @brief Get the window size object
 *
 * @return struct winsize
 */
struct winsize get_window_size() {
  struct winsize ws; // winsize holds two unsigned shorts, according to
                     // termios.h
  int fd;
  fd = open("/dev/tty", O_RDWR);
  if (fd < 0) {
    printf("Could not find terminal in /dev/tty \n");
    err(1, "/dev/tty");
  }
  if (ioctl(fd, TIOCGWINSZ, &ws) < 0) {
    printf("An error with ioctl that I did not google\n");
    err(1, "/dev/tty");
  }

  close(fd);
  return ws;
}

/**
 * @brief Get the term window object, create the terminal window for testing
 * purposes
 *
 * @param window A winsize struct to get the terminal bounds
 * @return char* To the terminal window buffer
 */
fastq_nucleotide *get_term_window(struct winsize window) {

  // get a warning from c++, not an issue in C
  fastq_nucleotide *term_window =
      malloc(TERM_SIZE(window.ws_col, window.ws_row) * sizeof(*term_window));

  for (size_t i = 0; i < TERM_SIZE(window.ws_row, window.ws_col); i++) {
    term_window[i] = EMPTY;
  }
  return term_window;
}

/**
 * @brief Return some random position in the row to populate with a character
 *
 * @param row_length The ws_row atrribute of the winsize struct
 * @return uint32_t The value of the row to fill
 */
uint32_t row_rand_position(unsigned short row_length) {
  uint32_t r = rand() % row_length;
  return r;
}

/**
 * @brief  Put some random characters into the first row of the terminal buffer
 *
 * @param term_buffer The array to be printed to the terminal
 * @param row_length The row length to be populated
 */
void test_populate_rows(char **term_buffer, unsigned short row_length) {
  uint32_t number_rows_fill =
      row_length * 0.1; // populate 10% of the columns with chars
  for (size_t i = 0; i < number_rows_fill; i++) {
    uint32_t val = row_rand_position(row_length);
    (*term_buffer)[val] = 'X'; // fill the buffer with some arbitrary value
  }
}

/**
 * @brief Increment wherever an x is moving it down the screen
 *
 * @param term A pointer to the start of the array
 * @param ws_ The winsize struct holding the array bounds.
 * @param row_val_ The row to place new information
 */
void test_increment_vals(char **term, const struct winsize *ws_,
                         const unsigned long int row_val_) {

  size_t term_size = TERM_SIZE(ws_->ws_col, ws_->ws_row);
  unsigned short row_val = row_val_ % ws_->ws_row;

  // Clear the bottome row out each time
  for (size_t g = ((ws_->ws_row * ws_->ws_col) - ws_->ws_col); g < term_size;
       g++) {
    (*term)[g] = FILL_CHAR;
  }
  for (size_t i = term_size; i != -1; i--) {
    if ((*term)[i] == 'X') {
      (*term)[i + ws_->ws_col] = 'X';
      (*term)[i] = FILL_CHAR; // bounds check not needed
    }
  }
}

int main() {
  struct winsize ws = get_window_size();
  fastq_nucleotides *fq_data = load_fastq("data/art_test1.fq");
  terminal_col *term_data = terminal_fastq_data(ws, fq_data->counter);
  fastq_nucleotide *window = get_term_window(ws);
  load_fastq_terminal(&term_data, &fq_data, ws);
  progress_terminal(&term_data, ws, &window);
  progress_terminal(&term_data, ws, &window);
  progress_terminal(&term_data, ws, &window);
  progress_terminal(&term_data, ws, &window);
  destroy_term_data(fq_data);
  destroy_term_fq(term_data, ws);
  free(fq_data);
  free(window);
  /*
  size_t term_size = TERM_SIZE(ws.ws_col, ws.ws_row); // should make this static
  so does not always need to be recalculated char* term = get_term_window(ws);
  test_populate_rows(&term, ws.ws_col); // passing col, as that is the number of
  rows fprintf(stderr, "Rows size %ud", ws.ws_row); initscr(); mvprintw(0, 0,
  term); refresh(); unsigned long int accu = 1; // skip first row, so row
  position is always positive uint8_t test_uptoval = 255; uint8_t t_val = 0;
  while (t_val != test_uptoval)
  {
      if((accu % 2) == 0){
          test_populate_rows(&term, ws.ws_col);
      }
      test_increment_vals(&term, &ws, accu);
      mvprintw(0, 0, term);
      refresh();
      ++accu;
      t_val++;
  }

  endwin();
  free(term);
  */
  return 0;
}
