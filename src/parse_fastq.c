/*
My first iteration of a fastq parser became overly convaluted, and kind of
messy. So I want to make fresh start and fix some of my mistakes

*/

// User defined headers
#include "parse_fastq.h"
#include "initialize_reads.h"
#include <string.h>

#define LINE_SIZE 256    // max length of a sequence
#define FASTQ_LOADS 1000 // how many entries to prepare
#define _SEQ_LEN_INIT_ 0;

/**
 *@brief Take in the sequence and quality string, and return a pointer to an
 *array of fastq nucleotide structs
 *
 *@param sequence_data The sequence data to input to the fastq_function
 *@param quality_data: The quality string to prepare the nucleotides
 *@return A pointer to the start of the fastq array
 **/
fastq_nucleotide *init_fastq_term(char *sequence_data, char *quality_data) {
  /*
      Function: init_fastq_term
      -------------------------
      Take in the sequence and quality string, and return a pointer to an array
     of fastq_nucleotide sturcts

      sequence_data: The sequence data to input to the fastq_function
      quality_data: The quality string to input to prepare the nucleotides for

      return: a pointer to the start of the fastq array
  */

  // only need enough room as is the length of one string as they benefit one
  // another
  fastq_nucleotide *terminal_data =
      malloc(sizeof(fastq_nucleotide) * strlen(sequence_data));

  if (strlen(sequence_data) == 0) {
    fprintf(stderr, "No seqeunce data\n");
    fprintf(stderr, "Error: %s\n", strerror(errno));
    exit(-1);
  }

  for (size_t i = 0; i < strlen(sequence_data); ++i) {
    terminal_data[i].color_pair = 4; // trying to get rid of valgrin warning
    terminal_data[i].nucleotide = sequence_data[i];
    terminal_data[i].quality_value = quality_data[i];
    terminal_data[i].color_pair =
        color_pair_val(terminal_data[i].quality_value);
    terminal_data[i].sequence_length = _SEQ_LEN_INIT_;
  }
  terminal_data[0].sequence_length = strlen(sequence_data);

  return terminal_data;
}

/**
 *@brief Free the memory in the double pointer struct fastq_nucleotides
 *
 *@param data_remove The data pointed to and to be removed
 * */
void destroy_term_data(fastq_nucleotides *data_remove) {

  for (size_t i = 0; i < data_remove->counter_copy; i++) {
    fprintf(stderr, "Removing: %c\t", data_remove->data[i]->nucleotide);
    free(data_remove->data[i]);
    fprintf(stderr, "Removed data\n");
  }
  free(data_remove->data);
  // free(data_remove);
  // fprintf(stderr, "Finished destroying terminal data");
}

/**
 *@brief Read the fastq file line by line, generateing an array of fastq
 *nucleotides
 *
 *@param fastq_data The file pointer to read the fastq data from
 *@return pointer to an array of type tbd
 */
fastq_nucleotides *init_fastq_data(FILE *fastq_data) {

  if (fastq_data == NULL) {
    fprintf(stderr, "Could not open file\n");
    fprintf(stderr, "Error: %s\n", strerror(errno));
    exit(-1);
  }
  // struct holds a double pointer to multiple arrays of fastq_nucleotide
  // structs
  fastq_nucleotides *term_data = malloc(sizeof(fastq_nucleotides));
  term_data->counter = 0;
  term_data->data = malloc(sizeof(fastq_nucleotide *) * FASTQ_LOADS);

  // make room to holde the sequences and qual sequences
  char sequence[LINE_SIZE];
  memset(sequence, 0, LINE_SIZE);
  char quality_sequence[LINE_SIZE];
  memset(sequence, 0, LINE_SIZE);

  char line[LINE_SIZE];
  uint_fast8_t header_ = 0;
  uint_fast8_t sequence_next = 0; // if sequence is next, be 1
  while (fgets(line, LINE_SIZE, fastq_data) != NULL) {
    if (line[0] == '@') {
      header_ = 1;
    } else if (header_ == 1 && strlen(sequence) == 0) {
      strcpy(sequence, line);
    } else if (sequence_next == 0) { // need a refresher of bools
      sequence_next = 1;
    } else if (sequence_next == 1) {
      strcpy(quality_sequence, line);
      if (strlen(quality_sequence) == strlen(sequence)) {
        term_data->data[term_data->counter] =
            init_fastq_term(sequence, quality_sequence);
        term_data->counter++;
      } else {
        fprintf(stderr, "Quality sequence and sequence length do not match\n");
        exit(-1);
      }
      header_ = 0;
      sequence_next = 0;
      memset(sequence, 0, LINE_SIZE);
    }
  }
  // destroy_term_data(term_data);
  fclose(fastq_data);
  term_data->counter_copy = term_data->counter;
  return term_data;
}

/**
 *@brief The function to load the fastq file, and handle the file pointer to
 *make the initial pointer fastq structure
 *
 *filename: The file name of the fastq file to be read
 */
fastq_nucleotides *load_fastq(char *filename) {

  FILE *fptr;
  fptr = fopen(filename, "r");
  if (fptr == NULL) {
    fprintf(stderr, "Could not open fastq file at %s\n", filename);
    fprintf(stderr, "Error: %s\n", strerror(errno));
    exit(-1);
  }

  fastq_nucleotides *term_data = init_fastq_data(fptr);
  return term_data;
}

// int main(int argc, char** argv){
//     if(argc == 2) load_fastq(argv[1]);
// }
