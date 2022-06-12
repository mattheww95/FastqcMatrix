/*
Prepare the reads struct and initialize them for batch processing to the screen
some other method will take care of posiontioning them.

I do not know yet if each base should be a struct or if each
struct should hold the array of data


Need to hold the color pair definitions in here too...

need to make a jumptable base on ranges, or switch may be best
*/
#include <ncurses.h>
#include <stdint.h>


// for new illumina only
// have 42 values to fill in
#define POOR_QUALITY 1
#define MODERATE_QUALITY 2
#define GOOD_QUALITY 3

uint8_t return_colour_pair(char basq){
    char base_num = baseq - '!'; // corresponds to 33 or 0 in encoding
    //higher is better
    switch (base_num)
    {
    case base_num > 13:
        return GOOD_QUALITY;
        break;
    
    default:
        break;
    }

};

typedef struct base_qc
{
    char* called_base;
    char* qulait_value;

}base_qc;
