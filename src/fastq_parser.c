/*
 * Re-writing alot of the program now that I am wiser with C
 * */

#include "fastq_parser.h"

// KSEQ_INIT(gzFile, gzread);
// extern volatile uint64_t sequence_count;

volatile uint64_t sequence_count = 0;
/**
 *@brief Read in the fastq file and prepare struct of fastq data
 *
 *@param inpath The path to the fastq file to open
 * */
kseq_t *read_file(const char *inpath) {
  gzFile fp = NULL;
  kseq_t *seq = NULL;
  int l = 0;
  uint64_t start_size = 40000;
  // extern volatile uint64_t sequence_count = 0;
  fp = gzopen(inpath, "r");
  seq = kseq_init(fp);
  // Initialize enought memory for 40000 sequences
  kseq_t *fastq_data = malloc(sizeof(*fastq_data) * start_size);
  while ((l = kseq_read(seq)) >= 0) {
    if (start_size == (sequence_count - 10)) {
      // fprintf(stderr, "Reallocating buffer size\n");
      start_size = start_size + start_size;
      kseq_t *fastq_data_ =
          realloc(fastq_data, sizeof(*fastq_data) * start_size);
      fprintf(stderr, "Loading more sequences.\n");
      if (fastq_data_ == NULL) {
        fprintf(stderr, "Could not reallocate buffer\n");
        exit(1);
      }
      fastq_data = fastq_data_;
    }
    kseq_t *sequence = malloc(sizeof(*sequence));
    // sequence = seq;
    kmemcpy(sequence, seq);
    // printf("Test: %s\n", seq->name.s);
    fastq_data[sequence_count] = *sequence;
    // printf("%s: %s\n", fastq_data[sequence_count]->name.s,
    //        fastq_data[sequence_count]->seq.s);
    sequence_count++;
  }
  fprintf(stderr, "Sequence count: %lu\n", sequence_count);
  fprintf(stderr, "Finished loading reads\n");
  kseq_destroy(seq);
  return fastq_data;
}

/**
 *@brief Create a copy of a kseq object
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

/**
 *@brief Free memory used by the reads
 *@param read_data the array of read data
 */
void destroy_reads(kseq_t *read_data) {
  for (size_t i = 0; i < sequence_count; i++) {
    fprintf(stderr, "%s: %s\n", read_data[i].name.s, read_data[i].seq.s);
    // kseq_destroy(&read_data[i]);
    free(read_data[i].name.s);
    free(read_data[i].comment.s);
    free(read_data[i].seq.s);
    free(read_data[i].qual.s);
    free(read_data[i].f);
  }
  free(read_data);
}

/*int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Could not open fastq file\n");
    return -1;
  }
  kseq_t *data = read_file(argv[1]);
  destroy_reads(data);
}
*/
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
