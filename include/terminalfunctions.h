#include <termios.h>
#include <unistd.h>

struct termios original;

extern void die(const char * message){
    perror(message);
    exit(1);
}

extern void enableRawMode(){
    struct termios raw;

    
    if (tcgetattr(STDIN_FILENO,&raw) == -1)
    {
        die("Error occured in : tcgetattr");
    }

    original = raw;
    raw.c_oflag &= ~(OPOST);
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == -1)
    {
        die("Error occured in : tcsetattr");
    }
}
extern void disableRawMode(){
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&original) == -1)
    {
        die("Error occured in : tcsetattr");
    }
}

