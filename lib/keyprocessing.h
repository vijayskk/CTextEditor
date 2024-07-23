#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../include/terminalfunctions.h"
#include "../include/defenitions.h"

extern void editorWindowRefresh();

struct inputConfig{
    int cx,cy;
    int firstInput;
};

struct inputConfig iC;

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
    iC.cx = 0;
    iC.cy = 0;
    iC.firstInput = 1;
    initTerminalWindow();
    enableRawMode();
    atexit(disableRawMode);
}

extern void quit(){
    disableRawMode();
}

void editorMoveCursor(int c){
    if (iC.firstInput == 1)
    {
        iC.firstInput = 0;
    }
    
    switch (c)
    {
    case ARROW_LEFT:
        if (iC.cx > 0)
        {
            iC.cx--;
        }
        
        break;
    
    case ARROW_RIGHT:

        if (iC.cx < getCol() - 1)
        {
            iC.cx++;
        }
        break;
    
    case ARROW_UP:
        if (iC.cy > 0)
        {
            iC.cy--;
        }
        break;

    case ARROW_DOWN:
        if (iC.cy < getRow() - 1)
        {
            iC.cy++;
        }
        break;
    }
}

void editorDrawRows(struct abuf * ab){
    int y;
    for (y = 0; y< getRow();y++){

        if (y == 0 && iC.firstInput == 1)
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

int editorReadkey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("Error reading...");
    }

    if (c == '\x1b')
    {
        char seq[3];
        if(read(STDIN_FILENO,&seq[0],1) != 1 ) return '\x1b';
        if(read(STDIN_FILENO,&seq[1],1) != 1 ) return '\x1b';

        if (seq[0] == '[')
        {
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if(read(STDIN_FILENO,&seq[2] , 1 ) != 1) return '\x1b';
                if (seq[2] == '~')
                {
                    switch (seq[1]){
                        case '1': return HOME_KEY;
                        case '3': return DELETE_KEY;
                        case '4': return END_KEY; 
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;  
                    }
                }
                
            }else{
                switch(seq[1]){
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
                case 'H': return HOME_KEY;
                case 'F': return END_KEY; 
            }
            }
            
            
        }else if(seq[0] == 'O'){
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
        return '\x1b';
    }else{
        return c;
    }
}

extern void editorProcessKeypress()
{
    int c = editorReadkey();

    switch (c)
    {
    case CTRL_KEY('x'):
        write(STDIN_FILENO,"\x1b[2J",4);
        write(STDIN_FILENO,"\x1b[H",3);
        exit(0);
        break;

    case HOME_KEY:
      iC.cx = 0;
      break;
    case END_KEY:
      iC.cx = getCol() - 1;
      break;

    case PAGE_UP:
    case PAGE_DOWN:
        {
            int times = getRow();
            while(times--) editorMoveCursor(c==PAGE_UP?ARROW_UP:ARROW_DOWN);
        }
        break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editorMoveCursor(c);
        break;
    case DELETE_KEY:
        editorMoveCursor(ARROW_LEFT);
    }
    editorWindowRefresh();
}

extern void editorWindowRefresh(){
    struct abuf * ab = (struct abuf *) malloc(sizeof(struct abuf));

    abAppend(ab, "\x1b[?25l", 6);
    abAppend(ab,"\x1b[H",3);

    editorDrawRows(ab);
    abAppend(ab,"\x1b[H",3);

    char cursorCmd[32];
    snprintf(cursorCmd,sizeof(cursorCmd),"\x1b[%d;%dH",iC.cy + 1,iC.cx + 1);
    abAppend(ab,cursorCmd,strlen(cursorCmd));
    abAppend(ab, "\x1b[?25h", 6);
    write(STDOUT_FILENO,ab->b,ab->len);
    free(ab);
}