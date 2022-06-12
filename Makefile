CC=gcc

bin/pxl_sze: src/create_buffer.c src/window_size.c
	$(CC) $^ -o $@ -lncurses

bin/test_fq: src/fastq_parser.c
	$(CC) $^ -o $@