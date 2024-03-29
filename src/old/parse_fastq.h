#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#ifndef _PARSE_FASTQ_
#define _PARSE_FASTQ_

typedef struct fastq_nucleotide{
    char nucleotide;
    char quality_value;
    uint8_t color_pair;
    uint32_t sequence_length; // A hack to track the line length
}fastq_nucleotide;

typedef struct fastq_nucleotides{
    fastq_nucleotide** data;
    uint32_t counter;
    uint32_t counter_copy; // added copy for as needs to be decremented twice
}fastq_nucleotides;

fastq_nucleotides* load_fastq(char*);

fastq_nucleotide* init_fastq_term(char*, char*);
void destroy_term_data(fastq_nucleotides*);
fastq_nucleotides* init_fastq_data(FILE*);

#endif
