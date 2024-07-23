#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../include/terminalfunctions.h"
#include "../include/defenitions.h"


struct abuf{
    char * b;
    int len;
};

#define ABUF_INIT {NULL, 0}


void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);
  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}
void abFree(struct abuf *ab) {
  free(ab->b);
}



extern void begin(){
    initTerminalWindow();
    enableRawMode();
    atexit(disableRawMode);
}

extern void quit(){
    disableRawMode();
}

void editorDrawRows(struct abuf * ab){
    int y;
    for (y = 0; y< getRow();y++){

        if (y == 0)
        {
            char welcome[80];
            int welcomelen = snprintf(welcome,sizeof(welcome),"%s -- version %s",STEXY_NAME,STEXY_VERSION);
            if(welcomelen > getCol()) welcomelen = getCol();
            int padding = (getCol() - welcomelen) / 2;
            if (padding)
            {
                abAppend(ab,"~",1);
                padding--;
            }
            while(padding--) abAppend(ab," ",1);
            abAppend(ab,welcome,welcomelen);
        }else{
            abAppend(ab,"~",1);
        }
    
        abAppend(ab, "\x1b[K", 3);
        if (y < getRow() - 1)
        {
            abAppend(ab,"\r\n",2);
        }
        
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
    struct abuf * ab = (struct abuf *) malloc(sizeof(struct abuf));

    abAppend(ab, "\x1b[?25l", 6);
    abAppend(ab,"\x1b[H",3);

    editorDrawRows(ab);
    abAppend(ab,"\x1b[H",3);

    abAppend(ab, "\x1b[?25h", 6);
    write(STDOUT_FILENO,ab->b,ab->len);
    free(ab);
}