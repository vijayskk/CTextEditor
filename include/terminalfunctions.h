#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct editorConfig
{
    int screenrows;
    int screencols;
    struct termios orig_terminoss;
};

struct editorConfig E;

int getWindowSize(int * rows,int * cols){
    struct winsize ws;

    // if (1 || ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws) == -1 || ws.ws_col == 0)
    // {
    //     if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    //     callback();
    //     return -1;
    // }else{
    //     *cols = ws.ws_col;
    //     *rows = ws.ws_row;
    //     return 0;
    // }
    

    if (ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws) == -1 || ws.ws_col == 0 )
    {
        return -1;
    }else{
        * rows = ws.ws_row;
        * cols = ws.ws_col;
        return 0;
    }
    
}
extern int getRow(){
    return E.screenrows;
}

extern void die(const char * message){
    write(STDIN_FILENO,"\x1b[2J",4);
    write(STDIN_FILENO,"\x1b[H",3);
    perror(message);
    exit(1);
}

void initTerminalWindow(){
    if (getWindowSize(&E.screenrows,&E.screencols) == -1)
    {
        die("Failed to fetch screen resolution.");
    }
    
}



extern void enableRawMode(){
    struct termios raw;

    
    if (tcgetattr(STDIN_FILENO,&raw) == -1)
    {
        die("Error occured in : tcgetattr");
    }

    E.orig_terminoss = raw;
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
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&E.orig_terminoss) == -1)
    {
        die("Error occured in : tcsetattr");
    }
}

