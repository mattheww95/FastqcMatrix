#include "window_size.h"


struct winsize get_window_size(){
    struct winsize ws;
    int fd;
    fd = open("/dev/tty", O_RDWR);
    if(fd < 0){
        printf("Could not find terminal in /dev/tty \n");
        err(1, "/dev/tty");
    }
    if(ioctl(fd, TIOCGWINSZ, &ws) < 0){
        printf("An error with ioctl that I did not google\n");
        err(1, "/dev/tty");
    }

    close(fd);
    return ws;

    
}

int rand_char();


/*
int main(){
    struct winsize ws;
    int fd;
    // Open the terminal in charge
    fd = open("/dev/tty", O_RDWR);

    if (fd < 0){
        err(1, "/dev/tty");
    }
    if(ioctl(fd, TIOCGWINSZ, &ws) < 0){ // no idea what this is doing
        err(1, "/dev/tty");
    }

    //printf("%d rows by %d columns\n", ws.ws_row, ws.ws_col);
    //printf("%d pixels by %d pixels\n", ws.ws_xpixel, ws.ws_ypixel); // pixel keeps coming up as zero...
    for(size_t f = 0; f < ws.ws_row; f++){
        for(size_t i = 0; i < ws.ws_col; i++){
             printf("%c", rand_char());
        }
        printf("\n");
    }
    close(fd);

}

int rand_char(){
    int r = rand(); 
    int ret_char = r % (26);
    return ret_char + 'A';
}
*/