/*
Parse a fastq file, 

format is:
    header
    sequence
    +
    quality string



need to make sure not loading entire file into memory at once
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
}fastq_line;

typedef struct fastq_data{
    fastq_line* fastq_data;
    __uint32_t entries;
}fastq_data;


fastq_data* initialize_fastq_entries(){
    fastq_data* data = malloc((LINE_SIZE * sizeof(fastq_data)));
    return data;
}


#define init_fastq 

int main(){
    FILE *fptr;
    fptr = fopen("data/art_test1.fq", "r");
    char line[LINE_SIZE];
    if(fptr == NULL){
        fprintf(stderr, "Could not open fastq file, exiting\n");
        fclose(fptr);
        exit(-1);
    }

    fprintf(stderr, "File open successfully\n");
    // make an array of fastq
    fastq_data* data = initialize_fastq_entries();
    while(fgets(line, LINE_SIZE, fptr)){
       __uint_least32_t line_length = strlen(line);
       // TODO deal with lines longer that 256 later

    }
    fclose(fptr);
    return 1;
}

