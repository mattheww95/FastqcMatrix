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
            (*tracer)->sequence = malloc(sizeof(*(*tracer)->sequence) * strlen(line));
            strcpy((*tracer)->sequence, line);
            printf("Sequence: %s\n", (*tracer)->sequence);

        }else if((*tracer)->header != NULL && (*tracer)->sequence != NULL && p_qual_time == 0){
            p_qual_time = 1;
            
        }else if(p_qual_time == 1){
            (*tracer)->quality_string = malloc(sizeof(*(*tracer)->quality_string) * strlen(line));
            strcpy((*tracer)->quality_string, line);
            p_qual_time = 0;
            (*tracer)->next = malloc(sizeof(*(*tracer)->next));
            tracer = &(*tracer)->next;
            (*tracer)->header = NULL;            
            (*tracer)->sequence = NULL;            
            (*tracer)->next = NULL;            
            (*tracer)->quality_string = NULL;            
        }
    }
    
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
   fastq_line* entry;

   while((*fastq_data)->header != NULL){ // header is null before next is
       printf("Header: %s \n", (*fastq_data)->header);
       printf("Sequence: %s \n", (*fastq_data)->sequence);
       printf("Quality Data: %s \n", (*fastq_data)->quality_string);
       entry = *fastq_data; 
       fastq_data = &(*fastq_data)->next;
       fprintf(stderr, "Freeing entry: %s\n", entry->header);
       free(entry);
   }
   fclose(fptr);
   free(*fastq_data);
}


int main(int argc, char** argv){
    load_fastq(argv[1]);
}

