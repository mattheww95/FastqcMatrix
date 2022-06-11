CC=gcc

bin/pxl_sze: src/window_size.c src/create_buffer.c
	$(CC) $^ -o $@