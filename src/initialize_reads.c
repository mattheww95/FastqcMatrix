/*
Prepare the reads struct and initialize them for batch processing to the screen
some other method will take care of posiontioning them.

I do not know yet if each base should be a struct or if each
struct should hold the array of data


Need to hold the color pair definitions in here too...

need to make a jumptable base on ranges, or switch may be best
*/
#include <initialize_reads.h>


// for new illumina only
// have 42 values to fill in

//---Define the colour pair values
#define POOR_QUALITY 1
#define MODERATE_QUALITY 2
#define GOOD_QUALITY 3
#define ERROR 4

//---define cut offs for quality
#define GOOD_QUALITY_SCORE '1'
#define MODERATE_QUALTIY_SCORE '.'
// No need to do a poor one as it wil defualt


uint8_t color_pair_val(char* baseq){
    if(baseq == NULL){
        fprintf(stderr, "Quality data does not exits\n");
        exit(-1);
    }
    if(*baseq > GOOD_QUALITY_SCORE){
        return GOOD_QUALITY;
    }else if(*baseq > MODERATE_QUALITY){
        return MODERATE_QUALITY;
    }else{
        return POOR_QUALITY;
    }
   return ERROR;
}

