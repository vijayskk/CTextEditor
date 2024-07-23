#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/terminalfunctions.h"
#include "../include/defenitions.h"




extern void begin(){
    initTerminalWindow();
    enableRawMode();
    atexit(disableRawMode);
}

extern void quit(){
    disableRawMode();
}

void editorDrawRows(){
    int y;
    for (y = 0; y< getRow();y++){
        write(STDIN_FILENO,"~\r\n",3);
    }
}

char editorReadkey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("Error reading...");
    }
    return c;
}

extern void editorProcessKeypress()
{
    char c = editorReadkey();

    switch (c)
    {
    case CTRL_KEY('x'):
        write(STDIN_FILENO,"\x1b[2J",4);
        write(STDIN_FILENO,"\x1b[H",3);
        exit(0);
        break;
    }
}

extern void editorWindowRefresh(){
    write(STDIN_FILENO,"\x1b[2J",4);
    write(STDIN_FILENO,"\x1b[H",3);

    editorDrawRows();
    write(STDIN_FILENO,"\x1b[H",3);
}