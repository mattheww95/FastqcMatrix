CC=gcc
CL=clang

bin/pxl_sze: src/create_buffer.c src/window_size.c
	$(CC) $^ -o $@ -lncurses -g

bin/test_fq: src/fastq_parser.c src/initialize_reads.c
	$(CC) $^ -o $@ -g -fanalyzer -lncurses -Isrc

fastq_parser: src/fastq_parser.c 
	$(CL) $^ -o $@ -g --analyze