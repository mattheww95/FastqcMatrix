
#ifndef __FASTQ_PARSER__
#define __FASTQ_PARSER__

#include "klib/kseq.h"
#include "klib/kstring.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

KSEQ_INIT(gzFile, gzread)
extern volatile uint64_t sequence_count;
void kmemcpy(kseq_t *dest, kseq_t *src);
kseq_t *read_file(const char *inpath);
void destroy_reads(kseq_t *read_data);

#endif
