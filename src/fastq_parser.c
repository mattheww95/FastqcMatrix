/*
 * Re-writing alot of the program now that I am wiser with C
 * */

#include "klib/kseq.h"
#include "klib/kstring.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

KSEQ_INIT(gzFile, gzread);
static uint64_t sequence_count = 0;
void kmemcpy(kseq_t *, kseq_t *);

/*
 *@brief Read in the fastq file and prepare struct of fastq data
 *
 *@param inpath The path to the fastq file to open
 * */
kseq_t **read_file(const char *inpath) {
  gzFile fp = NULL;
  kseq_t *seq = NULL;
  int l = 0;
  uint64_t start_size = 40000;
  fp = gzopen(inpath, "r");
  seq = kseq_init(fp);
  // Initialize enought memory for 40000 sequences
  kseq_t **fastq_data = malloc(sizeof(fastq_data) * start_size);
  while ((l = kseq_read(seq)) >= 0) {
    if (start_size == sequence_count) {
      fastq_data = realloc(fastq_data, start_size * 3);
      start_size = start_size * 3;
    }
    kseq_t *sequence = malloc(sizeof(*sequence));
    // sequence = seq;
    kmemcpy(sequence, seq);
    printf("Test: %s\n", seq->name.s);
    fastq_data[sequence_count] = sequence;
    // printf("%s: %s\n", fastq_data[sequence_count]->name.s,
    //        fastq_data[sequence_count]->seq.s);
    sequence_count++;
  }
  kseq_destroy(seq);
  return fastq_data;
}

/*@brief Create a copy of a kseq object
 *
 *@param dest The kseq_t object to be copy into
 *@param src The seq object the data is being copied from
 *
 */
void kmemcpy(kseq_t *dest, kseq_t *src) {
  // Copy the comment
  dest->comment.l = src->comment.l;
  dest->comment.m = src->comment.m;
  dest->comment.s = malloc(sizeof(char) * src->comment.l);
  memcpy(dest->comment.s, src->comment.s, src->comment.l);

  // copy name
  dest->name.l = src->name.l;
  dest->name.m = src->name.m;
  dest->name.s = malloc(sizeof(char) * src->name.l);
  memcpy(dest->name.s, src->name.s, src->name.l);

  // copy the sequence
  dest->seq.l = src->seq.l;
  dest->seq.m = src->seq.m;
  dest->seq.s = malloc(sizeof(char) * src->seq.l);
  memcpy(dest->seq.s, src->seq.s, src->seq.l);

  // copy the quality string
  dest->qual.l = src->qual.l;
  dest->qual.m = src->qual.m;
  dest->qual.s = malloc(sizeof(char) * src->qual.l);
  memcpy(dest->qual.s, src->qual.s, src->qual.l);

  dest->last_char = src->last_char;
}

/*
 *@brief Free memory used by the reads
 *
 */
void destroy_reads(kseq_t **read_data) {
  for (size_t i = 0; i < sequence_count; i++) {
    // printf("%s: %s\n", read_data[i]->name.s, read_data[i]->seq.s);
    kseq_destroy(read_data[i]);
  }
  free(read_data);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Could not open fastq file\n");
    return -1;
  }
  kseq_t **data = read_file(argv[1]);
  destroy_reads(data);
}

// int main(int argc, char **argv) {
//   gzFile fp = NULL;
//   kseq_t *seq = NULL;
//   int l = 0;
//   if (argc == 1) {
//     fprintf(stderr, "Usage: %s <in.fasta>\n", argv[0]);
//     return 1;
//   }
//   fp = gzopen(argv[1], "r");
//   seq = kseq_init(fp);
//   while ((l = kseq_read(seq)) >= 0) {
//     printf("name: %s\n", seq->name.s);
//     printf("length: %lu\n", seq->seq.l);
//     if (seq->comment.l)
//       printf("comment: %s\n", seq->comment.s);
//     printf("seq: %s\n", seq->seq.s);
//     if (seq->qual.l)
//       printf("qual: %s\n", seq->qual.s);
//   }
//   printf("return value: %d\n", l);
//   kseq_destroy(seq);
//   gzclose(fp);
//   return 0;
// }
