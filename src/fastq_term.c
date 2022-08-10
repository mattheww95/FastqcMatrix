/*Rework of the current fastq terminal to make better use of the reads and
 *some cleaner code
 *
 */

#include "fastq_term.h"
#include "fastq_parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Value to determine if a column should be filled or not
#define FILL_COLUMN_P 20

// The value to altering the maximum allowed value for the cool down counter
#define COOL_DOWN_MOD 10

// for new illumina only
// have 42 values to fill in

//---Define the colour pair values
#define POOR_QUALITY 1
#define MODERATE_QUALITY 2
#define GOOD_QUALITY 3
#define ERROR 4

//---define cut offs for quality
#define GOOD_QUALITY_SCORE '1'
#define MODERATE_QUALTIY_SCORE '.'
// No need to do a poor one as it wil defualt

//------GLOBALS------------
static struct winsize ws = {
    0, 0, 0, 0}; // The window size struct so it only needs to be computed once

static uint64_t window_size =
    0; // The total number of value that can fit on the terminal

/**
 *@brief Return the colour value based on the quality value associated with with
 *each called base
 *
 *@param baseq The quality value associated with each base
 */
uint8_t color_pair_val(char baseq) {

  if (baseq > GOOD_QUALITY_SCORE) {
    return GOOD_QUALITY;
  } else if (baseq > MODERATE_QUALITY) {
    return MODERATE_QUALITY;
  } else {
    return POOR_QUALITY;
  }
}

/**
 * @brief Get the window size object
 *
 * @return struct winsize
 */
void get_window_size() {
  // struct winsize _ws = {0, 0, 0, 0};
  // winsize holds two unsigned shorts, according to
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

  // keeping the window struct in memory so it does not need to be calculated
  // each time
  window_size = ws.ws_col * ws.ws_row;
  close(fd);
  // return _ws;
}

/**
 *@brief Create an array large enough to hold all of the nucleotide characters
 *to be displayed on screen
 *
 *@return A pointer to an array of read_char structs
 * */
read_char *init_term_read_buffer() {
  read_char *terminal = malloc(sizeof(*terminal) * window_size);
  return terminal;
}

/**
 *@brief Initialize the terminal_column array to feed the screen buffer
 *
 *@param the winsize struct containing the terminal width e.g. the number of
 *columns
 *@return An Array of emtpy structs
 * */
terminal_col *initialize_terminal(const struct winsize _ws) {
  terminal_col *term_data = malloc(sizeof(*term_data) * _ws.ws_col);
  for (size_t i = 0; i < ws.ws_col; i++) {
    term_data[i].column_idx = i;
    term_data[i].cool_down = i;
    term_data[i].is_empty = true;
    term_data[i].nucleotides = NULL;
    term_data[i].read_length = 0;
    term_data[i].array_pos = 0;
  }
  return term_data;
}

/**
 *@brief Convert a read quality sequence into a an array of read_char structs to
 *be loaded into the buffer to be displayed on the terminal
 *
 *@param A pointer to a fastq sequence of type kseq_t
 */
read_char *seq_to_read_char(kseq_t *sequence) {
  if (sequence->seq.l != sequence->qual.l) {
    fprintf(stderr,
            "Sequence length does not equal quality length for read: %s\n",
            sequence->name.s);
  }
  read_char *read_value =
      malloc(sizeof(*read_value) * sequence->seq.l); // initialize read values
  for (size_t i = 0; i < sequence->seq.l; i++) {
    read_value[i].nucleotide = sequence->seq.s[i];
    read_value[i].colour_value = color_pair_val(sequence->qual.s[i]);
  }
  return read_value;
}

/**
 *@brief check the terminal columns to determine if they are empty/finished
 *cooling down and load another set of sequences if needed
 *
 *@param terminal_columns A pointer to an array of the terminal columns to be
 *loaded
 *@param fastq_reads The reads to be loaded into the terminal structs
 **/
terminal_col *load_terminal_columns(terminal_col *terminal_columns,
                                    kseq_t *fastq_reads,
                                    uint64_t sequence_count) {
  // extern volatile uint64_t sequence_count;
  // left off here
  for (size_t i = 0; i < ws.ws_col; i++) {
    if (terminal_columns[i].cool_down == 0 && terminal_columns[i].is_empty) {
      if (sequence_count == 0) {
        fprintf(stderr, "You have reached the end of your fastq file!\n");
        // Need to add in a GOTO statement here to trigger destructor element
        // TODO Create goto statement for this clean up or better yet a clenaup
        // function
        printf("\n"); // puting this in for now
        exit(1);
      }
      uint64_t rand_value = get_rand();
      if (rand_value > FILL_COLUMN_P) {
        terminal_columns[i].cool_down = (get_rand() % COOL_DOWN_MOD);
        terminal_columns[i].array_pos = 0;
        fprintf(stderr, "%s\n", fastq_reads[sequence_count].seq.s);
        terminal_columns[i].nucleotides =
            seq_to_read_char(&fastq_reads[sequence_count]);
        // This destructor will likely be a potential source of bugs
        terminal_columns[i].read_length = fastq_reads[sequence_count].seq.l;
        // TODO figure out why this throws an error
        // kseq_destroy(&fastq_reads[sequence_count]);
        sequence_count--;
        terminal_columns[i].is_empty = false;
      }
    }
  }
  return terminal_columns;
}

/**
 *@brief Fill the display buffer with the loaded terminal columns
 *
 *@param display_buffer_ Pointer to the buffer to be displayed on the screen,
 *passed by reference
 *@param loaded_columns The terminal columns loaded with reads to display to the
 *screen
 **/
void load_display_buffer(read_char **display_buffer_,
                         terminal_col *loaded_columns) {
  // This function needs to decrement the cooldown counter
  // fill the terminal display buffer
  read_char *display_buffer = *(display_buffer_);
  for (size_t i = 0; i < ws.ws_col; i++) {
    if (loaded_columns[i].nucleotides == NULL) {
      continue;
    } else if (!loaded_columns[i].is_empty &&
               loaded_columns[i].array_pos != loaded_columns[i].read_length) {
      // while the column is not empty add new nucleotides to the screen
      display_buffer[i] =
          loaded_columns[i].nucleotides[loaded_columns[i].array_pos];
      loaded_columns[i].array_pos++;
      if (loaded_columns[i].array_pos == loaded_columns[i].read_length) {
        loaded_columns[i].nucleotides = NULL;
      }
    } else if (!loaded_columns[i].is_empty && loaded_columns[i].cool_down > 0) {
      // once the column has exhausted all nucleotides in the array start
      // decrementing the cool down
      loaded_columns[i].cool_down--;
    } else {
      // once the cooldown is decremented set the column to empty to be refilled
      loaded_columns[i].is_empty = true;
    }
  }
}

void destroy_terminal_columns(terminal_col *term_data) {
  for (size_t i = 0; i < ws.ws_col; i++) {
    free(term_data[i].nucleotides);
  }
  free(term_data);
}

uint64_t get_rand() {
  uint64_t random_value = (uint64_t)(rand() % ws.ws_col);
  return random_value;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "No file input specified\n");
    return -1;
  }
  get_window_size(); // Initialize winsize struct
  if (ws.ws_col == 0 || window_size == 0) {
    fprintf(stderr, "Winsize struct not properly initiailized\n");
    fprintf(stderr, "Dimensions %d x %d\n", ws.ws_col, ws.ws_row);
    return -1;
  }
  extern volatile uint64_t sequence_count;
  read_char *terminal_screen = init_term_read_buffer(); // The terminal buffer
  printf("fastq term sequence count:%lu\n", sequence_count);
  kseq_t *read_data = read_file(argv[1]);
  sequence_count--; // checking if this ends errors andd it does!
  printf("fastq term sequence count:%lu\n", sequence_count);
  terminal_col *term_data = initialize_terminal(ws);
  term_data = load_terminal_columns(term_data, read_data, sequence_count);
  load_display_buffer(&terminal_screen, term_data);
  // destroy_reads(read_data);
  destroy_terminal_columns(term_data);
  free(read_data);
  return 0;
}
