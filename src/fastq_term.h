#ifndef __FASTQ_TERM__
#define __FASTQ_TERM__

#include "fastq_parser.h"
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

void get_window_size(); // call first to compute the ws struct
// static struct winsize ws = {
//   0, 0, 0, 0}; // The window size struct so it only needs to be computed once

// static uint64_t window_size =
//   0; // The total number of value that can fit on the terminal

typedef struct read_char {
  char nucleotide;
  char quality_value;
  uint16_t colour_value;

} read_char;

typedef struct terminal_col {
  bool is_empty;
  uint16_t cool_down;
  uint64_t column_idx;
  read_char *nucleotides;

} terminal_col;

read_char *init_term_read_buffer(); // Create a buffer on the terminal for the
                                    // reads to be displayed
terminal_col *initialize_terminal(
    const struct winsize _ws); // Create the structs for the terminal

uint8_t color_pair_val(
    char baseq); // Take the correct colour value for each of the quality values

read_char *seq_to_read_char(kseq_t *sequence); // Convert a kseq_t object into a
                                               // read colour value object
uint64_t get_rand(); // The value used to random colour pairings

// Fill the terminal columns with read data
terminal_col *load_terminal_columns(terminal_col *terminal_columns,
                                    kseq_t *fastq_reads,
                                    uint64_t sequence_count);
#endif
