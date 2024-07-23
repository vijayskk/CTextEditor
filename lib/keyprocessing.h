#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../include/terminalfunctions.h"
#include "../include/defenitions.h"

extern void editorWindowRefresh();
char * fname = NULL;
typedef struct erow
{
    int size;
    char *chars;
} erow;

struct editorData
{
    int numrows;
    erow * row;
    int rowOffset;
    int colOffset;
};

struct inputConfig
{
    int cx, cy;
    int firstInput;
};

struct inputConfig iC;
struct editorData eD;

struct abuf
{
    char *b;
    int len;
};

#define ABUF_INIT \
    {             \
        NULL, 0   \
    }

void abAppend(struct abuf *ab, const char *s, int len)
{
    char *new = realloc(ab->b, ab->len + len);
    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}
void abFree(struct abuf *ab)
{
    free(ab->b);
}

extern void begin()
{
    iC.cx = 0;
    iC.cy = 0;
    iC.firstInput = 1;
    eD.numrows = 0;
    eD.row = NULL;
    eD.rowOffset = 0;
    eD.colOffset = 0;
    initTerminalWindow();
    E.screenrows -= 1;
    enableRawMode();
    atexit(disableRawMode);
}

extern void quit()
{
    disableRawMode();
}

void editorMoveCursor(int c)
{
    erow * row = (iC.cy >= eD.numrows)?NULL:&eD.row[iC.cy];

    if (iC.firstInput == 1)
    {
        iC.firstInput = 0;
    }

    switch (c)
    {
    case ARROW_LEFT:
        if (iC.cx != 0)
        {
            iC.cx--;
        }else if (iC.cy > 0){
            iC.cy--;
            iC.cx = eD.row[iC.cy].size;
        }

        break;

    case ARROW_RIGHT:
        if (row && iC.cx < row->size)
        {
            iC.cx++;
        }else if (row && iC.cx == row->size){
            iC.cy++;
            iC.cx = 0;
        }
        break;

    case ARROW_UP:
        if (iC.cy > 0)
        {
            iC.cy--;
        }
        break;

    case ARROW_DOWN:
        if (iC.cy < eD.numrows)
        {
            iC.cy++;
        }
        break;
    }

    row = (iC.cy >= eD.numrows) ? NULL : &eD.row[iC.cy];
    int rowlen = row ? row->size : 0;
    if (iC.cx > rowlen) {
        iC.cx = rowlen;
    }
}

void editorDrawRows(struct abuf *ab)
{
    int y;
    for (y = 0; y < getRow(); y++)
    {
        int filerow = y + eD.rowOffset;
        if (filerow >= eD.numrows)
        {
            if (y == 0 && iC.firstInput == 1)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "%s -- version %s", STEXY_NAME, STEXY_VERSION);
                if (welcomelen > getCol())
                    welcomelen = getCol();
                int padding = (getCol() - welcomelen) / 2;
                if (padding)
                {
                    abAppend(ab, "~", 1);
                    padding--;
                }
                while (padding--)
                    abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            }
            else
            {
                abAppend(ab, "~", 1);
            }
        }else{
            int len = eD.row[filerow].size - eD.colOffset;
            if (len<0) len = 0;
            if (len > getCol())
            {
                len = getCol();
            }
            abAppend(ab,&eD.row[filerow].chars[eD.colOffset],len);
        }
        

        abAppend(ab, "\x1b[K", 3);
        abAppend(ab, "\r\n", 2);
        
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
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';

        if (seq[0] == '[')
        {
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';
                if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                    case '1':
                        return HOME_KEY;
                    case '3':
                        return DELETE_KEY;
                    case '4':
                        return END_KEY;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME_KEY;
                    case '8':
                        return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O')
        {
            switch (seq[1])
            {
            case 'H':
                return HOME_KEY;
            case 'F':
                return END_KEY;
            }
        }
        return '\x1b';
    }
    else
    {
        return c;
    }
}

extern void editorProcessKeypress()
{
    int c = editorReadkey();

    switch (c)
    {
    case CTRL_KEY('x'):
        write(STDIN_FILENO, "\x1b[2J", 4);
        write(STDIN_FILENO, "\x1b[H", 3);
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
        while (times--)
            editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
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


void editorScroll() {
  if (iC.cy < eD.rowOffset) {
    eD.rowOffset = iC.cy;
  }
  if (iC.cy >= eD.rowOffset + getRow()) {
    eD.rowOffset = iC.cy - getRow() + 1;
  }
  if (iC.cx < eD.colOffset) {
    eD.colOffset = iC.cx;
  }
  if (iC.cx >= eD.colOffset + getCol()) {
    eD.colOffset = iC.cx - getCol() + 1;
  }
}

void editorDrawStatusBar(struct abuf *ab) {
  abAppend(ab, "\x1b[7m", 4);
  char status[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines      Ctrl+X to exit",
  fname ? fname : "[No Name]", eD.numrows);
  if (len > E.screencols) len = E.screencols;
  abAppend(ab, status, len);
  while (len < E.screencols) {
    abAppend(ab, " ", 1);
    len++;
  }
  abAppend(ab, "\x1b[m", 3);
}

extern void editorWindowRefresh()
{
    editorScroll();
    struct abuf *ab = (struct abuf *)malloc(sizeof(struct abuf));

    abAppend(ab, "\x1b[?25l", 6);
    abAppend(ab, "\x1b[H", 3);

    editorDrawRows(ab);
    editorDrawStatusBar(ab);
    abAppend(ab, "\x1b[H", 3);

    char cursorCmd[32];
    snprintf(cursorCmd, sizeof(cursorCmd), "\x1b[%d;%dH", (iC.cy - eD.rowOffset ) + 1, iC.cx + 1);
    abAppend(ab, cursorCmd, strlen(cursorCmd));
    abAppend(ab, "\x1b[?25h", 6);
    write(STDOUT_FILENO, ab->b, ab->len);
    free(ab);
}

void editorAppendRow(char * s,size_t len){
    eD.row = realloc(eD.row,sizeof(erow) * (eD.numrows + 1));

    int at = eD.numrows;
    eD.row[at].size = len;
    eD.row[at].chars = malloc(len + 1);
    memcpy(eD.row[at].chars,s,len);
    eD.row[at].chars[len] = '\0';
    eD.numrows++;
}

extern void editorOpen(const char * filename)
{
    FILE * fp = fopen(filename,"r");
    if(!fp) die("fopen");
    fname = strdup(filename);
    iC.firstInput = 0;
    char * line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while (( linelen = getline(&line,&linecap,fp)) != -1)
        {
            while (linelen > 0 && ( line[linelen - 1] == '\n' || line[linelen - 1] == '\r' ))
            {
                linelen--;
            }
            editorAppendRow(line,linelen);
        }

    free(line);
    fclose(fp);
}