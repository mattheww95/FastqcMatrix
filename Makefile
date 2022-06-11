CC=gcc

bin/pxl_sze: src/create_buffer.c src/window_size.c
	$(CC) $^ -o $@ -lncurses