CC=gcc
CL=clang

bin/pxl_sze: src/create_buffer.c src/window_size.c
	$(CC) $^ -o $@ -lncurses -g

bin/test_fq: src/parse_fastq.c src/initialize_reads.c
	$(CC) $^ -o $@ -g -fanalyzer -lncurses -Isrc

bin/fastq_parser: src/parse_fastq.c src/initialize_reads.c
	$(CL) $^ -o $@ -lncurses -Isrc

bin/fastq_term: src/fastq_term.c src/parse_fastq.c src/initialize_reads.c
	$(CC) $^ -o $@ -g -lncurses -Isrc

bin/test_kseq: src/fastq_parser.c src/fastq_term.c
	$(CC) $^ -o $@ -g -lz -Wall -pedantic -lncurses
