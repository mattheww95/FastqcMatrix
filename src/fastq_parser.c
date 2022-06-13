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



fastq_line* init_fastq_data(FILE* some_file_ptr){
    // iniitialize the fastq data struct
    char line[LINE_SIZE];
    bool p_fql_hdr = false;
    bool p_seq = false;
    bool p_qual_time = false;

    fastq_line* node1 = malloc(sizeof(node1));
    printf("alloc'd node1\n");
    node1->next = NULL;

    fastq_line* start_of_list = malloc(sizeof(start_of_list));
    start_of_list = node1;

    while(fgets(line, LINE_SIZE, some_file_ptr)) {
        // TODO this can be cleaner as the standard comes in threes
        if(p_fql_hdr && line[0] == '@'){
            // start the reading
            node1->header = line;
            p_fql_hdr = false;
        }else if(p_fql_hdr && !p_seq){
           node1->sequence = line;
           p_seq == false; 
        }else if(line[0] == '+'){
            p_qual_time = true;
        }else if(p_seq & p_qual_time & p_fql_hdr){
            node1->quality_string = line;
            fastq_line* node2 = malloc(sizeof(node2));
            node1->next = node2;
            p_fql_hdr = false;
            p_seq = false;
            p_qual_time = false;
        }

    }

    return start_of_list;
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
   fastq_line* fastq_data = init_fastq_data(fptr);
   fastq_line* temp;
   temp = fastq_data;
   while(temp->next != NULL){
       printf("Header: %s \n", fastq_data->header);
       printf("Sequence: %s \n", fastq_data->sequence);
       printf("Quality Data: %s \n", fastq_data->quality_string);
       temp->next = fastq_data;
       free(fastq_data);
   }
   free(temp);
}


int main(int argc, char** argv){
    load_fastq(argv[1]);
}

