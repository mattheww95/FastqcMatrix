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

#define LINE_SIZE 256

typedef struct fastq_line{
    char* header;
    char* sequence;
    char* quality_string;
    struct fastq_line* next;
}fastq_line;



fastq_line** init_fastq_data(FILE* some_file_ptr){
    // iniitialize the fastq data struct
    char line[LINE_SIZE];
    bool p_fql_hdr = false;
    bool p_seq = false;
    bool p_qual_time = false;

    fastq_line* node1 = malloc(sizeof(node1));
    
    fastq_line** tracer = malloc(sizeof(tracer));
    fastq_line** start_of_line = malloc(sizeof(start_of_line));
    tracer = &node1;
    start_of_line = &node1;

    while(fgets(line, LINE_SIZE, some_file_ptr)) {
        // TODO this can be cleaner as the standard comes in threes
        if(line[0] == '@'){
            fprintf(stderr, "Header: %s\n", line);
            (*tracer)->header = malloc(sizeof((*tracer)->header) * strlen(line));
            strcpy((*tracer)->header, line);
            p_fql_hdr = true;

        }else if(!p_seq && (*tracer)->header != NULL){
           (*tracer)->sequence = malloc(sizeof((*tracer)->sequence) * strlen(line));
           strcpy((*tracer)->sequence, line);
           printf("Sequence: %s\n", (*tracer)->sequence);
        
        }else if(p_seq && line[0] != '+'){
            (*tracer)->quality_string = malloc(sizeof((*tracer)->quality_string) * strlen(line));
            strcpy((*tracer)->quality_string, line);
            printf("Quality String: %s\n", line);
            (*tracer)->next = malloc(sizeof((*tracer)->next));
        }

    }
    free(node1);
    return start_of_line;
}


void load_fastq(char* filename){
    // Load a fastq file and return the initialize a fastq data struct

    FILE *fptr;
    fptr = fopen(filename, "r");
    if(fptr == NULL){
        fprintf(stderr, "Could not open fastq file\n");
        free(fptr);
        exit(-1);
    }
   fastq_line** fastq_data = init_fastq_data(fptr);
   fastq_line* entry = malloc(sizeof(entry));
   entry = *fastq_data;
   // something feels wrong..

   while((*fastq_data)->next != NULL){
       printf("Header: %s \n", entry->header);
       printf("Sequence: %s \n", entry->sequence);
       printf("Quality Data: %s \n", entry->quality_string);
       fastq_data = &entry->next;
       entry = *fastq_data;
   }
   //free(entry);
   //free(fastq_data);
}


int main(int argc, char** argv){
    load_fastq(argv[1]);
}

