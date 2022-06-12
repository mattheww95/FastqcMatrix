/*
Parse a fastq file, 

format is:
    header
    sequence
    +
    quality string

*/

#include <stdio.h>
#include <stdlib.h>



int main(){
    FILE *fptr;
    fptr = fopen("data/art_test1.fq", "r");
    if(fptr == NULL){
        fprintf(stderr, "Could not open fastq file, exiting\n");
        fclose(fptr);
        exit(-1);
    } 
    fprintf(stderr, "File open successfully\n");
    fclose(fptr);
    return 1;
}