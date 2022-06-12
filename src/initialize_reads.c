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

//---Define the colour pair values
#define POOR_QUALITY 1
#define MODERATE_QUALITY 2
#define GOOD_QUALITY 3


//--Define cut offs for quality
#define GOOD_QUALITY_SCORE '5'
#define MODERATE_QUALTIY_SCORE '.'
// No need to do a poor one as it wil defualt



uint8_t return_colour_pair(char basq){
    //char base_num = baseq - '!'; // corresponds to 33 or 0 in encoding
    //higher is better
    
    uint8_t ret_val;
    switch (base_q)
    {
    case base_num > GOOD_QUALITY_SCORE;
        ret_val = GOOD_QUALITY;
        break;
    case base_num > MODERATE_QUALITY_SCORE;
        ret_val = MODERATE_QUALITY;
        break;
    
    default:
        ret_val = POOR_QUALITY;
        break;
    }
    return ret_val;
}

typedef struct base_qc
{
    char* called_base;
    char* qulait_value;
    uint8_t color_pair_val;

}base_qc;
