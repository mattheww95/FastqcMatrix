/*
My first iteration of a fastq parser became overly convaluted, and kind of
messy. So I want to make fresh start and fix some of my mistakes

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// User defined headers
#include <initialize_reads.h>


#define LINE_SIZE 256 // max length of a sequence
#define FASTQ_LOADS 1000 // how many entries to prepare

typedef struct fastq_nucleotide{
    char nucleotide;
    char quality_value;
    uint8_t color_pair;
}fastq_nucleotide;

typedef struct fastq_nucleotides{
    fastq_nucleotide** data;
    uint32_t counter;
}fastq_nucleotides;

//typedef struct fastq_line{
//    char* header;
//    char* sequence;
//    char* quality_string;
//    struct fastq_line* next;
//}fastq_line;

fastq_nucleotide* init_fastq_term(char* sequence_data, char* quality_data){
    /*
        Function: init_fastq_term
        -------------------------
        Take in the sequence and quality string, and return a pointer to an array of 
        fastq_nucleotide sturcts 

        sequence_data: The sequence data to input to the fastq_function
        quality_data: The quality string to input to prepare the nucleotides for

        return: a pointer to the start of the fastq array 
    */

   // only need enough room as is the length of one string as they benefit one another
   fastq_nucleotide* terminal_data = malloc(sizeof(fastq_nucleotide) * strlen(sequence_data));
   if(strlen(sequence_data) == 0){
       fprintf(stderr, "No seqeunce data\n");
       exit(-1);
   }

    for(size_t i = 0; i < strlen(sequence_data); ++i){
        terminal_data[i].nucleotide = sequence_data[i];
        terminal_data[i].quality_value = quality_data[i]; 
        terminal_data[i].color_pair = color_pair_val(&quality_data[i]);
    }
    return terminal_data;

}

void destroy_term_data(fastq_nucleotides* data_remove){
    /*
        Function: destroy_term_data
        ---------------------------
        Free the memory in the double pointer struct fastq_nucleotides

        data_remove: The data to remove

        return: void
    */

   for(size_t i = 0; i < data_remove->counter; ++i){
       free(data_remove->data[i]);
   }
   free(data_remove);

}

void init_fastq_data(FILE* fastq_data){
    /*
        Function: init_fastq_term
        -------------------------
        Read the fastq file line by line, generating an array of fastq nucleotides

        fastq_data: The file pointer to read the fastq data from

        return: a pointer to an array of a TBD type 
    */
   if(fastq_data == NULL){
       fprintf(stderr, "Could not open file\n");
       exit(-1);

   }
   // struct holds a double pointer to multiple arrays of fastq_nucleotide structs
   fastq_nucleotides* term_data = malloc(sizeof(fastq_nucleotides));
   term_data->counter = 0;
   term_data->data = malloc(sizeof(fastq_nucleotide*) * FASTQ_LOADS);

    // make room to holde the sequences and qual sequences
   char sequence[LINE_SIZE];
   memset(sequence, 'c', LINE_SIZE);
   char quality_sequence[LINE_SIZE];
   memset(sequence, 'c', LINE_SIZE);
   char line[LINE_SIZE];
   uint_fast8_t header_ = 0;
   uint_fast8_t sequence_next = 0; // if sequence is next, be 1
   while(fgets(line, LINE_SIZE, fastq_data)){
       if(line[0] == '@'){
           header_ = 1;
       }else if(header_ && strlen(sequence) == 0){
           strcpy(sequence, line);
       }else if(!sequence_next){
           sequence_next = 1;
       }else if(sequence_next){
           strcpy(quality_sequence, line);
           if(strlen(quality_sequence) > 0 && strlen(sequence) > 0 ){
               term_data->data[term_data->counter] = init_fastq_term(sequence, quality_sequence);
               term_data->counter++;
            }
           memset(sequence, 'C', LINE_SIZE);

       }
   }
   destroy_term_data(term_data);

}


void load_fastq(char* filename){
/*
    Function: load_fastq
    --------------------

    The function to load the fastq file, handle the file pointer and
    create the initial fastq structure

    filename: The file name of the fastq file to be read in

    return: void
*/
    FILE* fptr;
    fptr = fopen(filename, "r");
    if(fptr == NULL){
        fprintf(stderr, "Could not open fastq file at %s\n", filename);
        exit(-1);
    }

    init_fastq_data(fptr);

    fclose(fptr);

}

int main(int argc, char** argv){
    if(argc == 1) load_fastq(argv[1]);
}
