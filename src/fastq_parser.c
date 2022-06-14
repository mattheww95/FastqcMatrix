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
    node1->next = NULL;

    fastq_line** tracer = malloc(sizeof(tracer));
    fastq_line** start_of_line = malloc(sizeof(start_of_line));
    tracer = &node1;
    start_of_line = &node1;

    while(fgets(line, LINE_SIZE, some_file_ptr)) {
        // TODO this can be cleaner as the standard comes in threes
        if(!p_fql_hdr && line[0] == '@'){
            // start the reading
            printf("Header: %s\n", line);
            node1->header = line;
            p_fql_hdr = false;

        }else if(p_fql_hdr && !p_seq){
           node1->sequence = line;
           p_seq == true; 
           printf("Sequence: %s\n", line);

        }else if(line[0] == '+'){
            p_qual_time = true;
            printf("Plus: %s\n", line);

        }else if(p_seq & p_qual_time & p_fql_hdr){
            node1->quality_string = line;
            printf("Quality String: %s\n", line);
            fastq_line* node2 = malloc(sizeof(node2));
            node1->next = node2;
            tracer = &node1->next;

            p_fql_hdr = false;
            p_seq = false;
            p_qual_time = false;
        }

    }

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

