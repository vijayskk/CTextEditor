#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/terminalfunctions.h"
#include "../include/defenitions.h"

extern void begin(){
    enableRawMode();
    atexit(disableRawMode);
}

extern void quit(){
    disableRawMode();
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
        exit(0);
        break;
    }
}