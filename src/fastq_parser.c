/*
Parse a fastq file, 

format is:
    header
    sequence
    +
    quality string



need to make sure not loading entire file into memory at once
going to make this a module for loading and preparing fastq data
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // need to wrap this in ifndef for gnu vs posix
#include <stdbool.h>
#include <stdint.h>
#include <initialize_reads.h> 

#define LINE_SIZE 256
#define FASTQ_LOADS 1000

typedef struct fastq_nucleotide{
    char nucleotide;
    char quality_value;
    uint8_t color_pair;
}fastq_nucleotide;

typedef struct fastq_nucleotides{
    fastq_nucleotide** data;
    uint32_t counter;
}fastq_nucleotides;

typedef struct fastq_line{
    char* header;
    char* sequence;
    char* quality_string;
    struct fastq_line* next;
}fastq_line;



void init_fastq_data(FILE* some_file_ptr, fastq_line* node1){
    // iniitialize the fastq data struct
    char line[LINE_SIZE];
    int p_qual_time = 0;

    node1->header = NULL;
    node1->sequence = NULL;
    node1->quality_string = NULL;
    node1->next = NULL;    
    fastq_line** tracer;// = malloc(sizeof(tracer));
    tracer = &node1;

    while(fgets(line, LINE_SIZE, some_file_ptr)) {

        if(line[0] == '@'){
            (*tracer)->header = malloc(sizeof(*(*tracer)->header) * strlen(line));
            strcpy((*tracer)->header, line);

        }else if((*tracer)->header != NULL && (*tracer)->sequence == NULL){
            (*tracer)->sequence = malloc(sizeof((*tracer)->sequence) * strlen(line));
            strcpy((*tracer)->sequence, line);
            //printf("Sequence: %s\n", (*tracer)->sequence);

        }else if((*tracer)->header != NULL && (*tracer)->sequence != NULL && p_qual_time == 0){
            p_qual_time = 1;
            
        }else if(p_qual_time == 1){
            (*tracer)->quality_string = malloc(sizeof(*(*tracer)->quality_string) * strlen(line));
            strcpy((*tracer)->quality_string, line);
            p_qual_time = 0;
            (*tracer)->next = malloc(sizeof((*tracer)->next));
            tracer = &(*tracer)->next;
            (*tracer)->header = NULL;            
            (*tracer)->sequence = NULL;            
            (*tracer)->next = NULL;            
            (*tracer)->quality_string = NULL;            
        }
    }
    
}


fastq_nucleotide* reads_to_fastq(fastq_line* fastq_data_){
    // Create an array of arrays holding struct info for each sequence
    // this can definately be done earlier...
    // make a struct array
    fastq_nucleotide* seq_data = malloc(sizeof(fastq_nucleotide) * strlen(fastq_data_->quality_string));
    //memset(seq_data, 0, strlen(fastq_data_->quality_string) * sizeof(fastq_nucleotide*));
    for(size_t i = 0; i < strlen(fastq_data_->sequence); ++i){ 
        seq_data[i].nucleotide = fastq_data_->sequence[i];
        seq_data[i].quality_value = fastq_data_->quality_string[i];
        seq_data[i].color_pair = color_pair_val(&fastq_data_->quality_string[i]);
    }
    return seq_data;

}

void destroy_fastq_term(fastq_nucleotides data){
    // Free the memory used to display the data to the terminal
    for(size_t i = 0; i < data.counter;++i){
        free(data.data[i]);
    }
    
}

void destroy_fastq_line(fastq_line* entry){
    free(entry->header);
    free(entry->quality_string);
    free(entry->sequence);
}


void load_fastq(char* filename){
    // Load a fastq file and return the initialize a fastq data struct

   FILE *fptr;
   fptr = fopen(filename, "r");
   if(fptr == NULL){
       fprintf(stderr, "Could not open fastq file\n");
       exit(-1);
   }
   fastq_line* node = malloc(sizeof(*node));
   fastq_line** fastq_data = NULL;
   fastq_data = &node;
   init_fastq_data(fptr, node); // pasing in double pointer as to not have dangling pointer or stale stack frame
   if(fastq_data == NULL){
       fprintf(stderr, "No data returned\n");
       exit(-1);
   }

   fastq_nucleotides fastq_term;
   fastq_term.counter = 0;
   fastq_term.data  = malloc(sizeof(fastq_nucleotide*) * FASTQ_LOADS); // malloc enough for 500 lines while playing:
   

   fastq_line* entry;
    // this is to destruct the linked list, will probaby move it into its own method
   while((*fastq_data)->next != NULL){
       //printf("Header: %s \n", (*fastq_data)->header);
       //printf("Sequence: %s \n", (*fastq_data)->sequence);
       //printf("Quality Data: %s \n", (*fastq_data)->quality_string);
       fastq_term.data[fastq_term.counter] = reads_to_fastq(*fastq_data);
       fastq_term.counter += 1;

       entry = *fastq_data;
       fastq_data = &(*fastq_data)->next;
       //fprintf(stderr, "Freeing entry: %s\n", entry->header);
       destroy_fastq_line(entry);
   }
   destroy_fastq_term(fastq_term);
   //free(*fastq_data);
   fclose(fptr);
}






int main(int argc, char** argv){
    load_fastq(argv[1]);
}

