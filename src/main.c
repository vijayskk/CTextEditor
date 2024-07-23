#include "../lib/keyprocessing.h"



int main()
{
    
    begin();
    editorOpen();
    while (1)
    {
        editorWindowRefresh();
        editorProcessKeypress();
    }
    quit();
    return 0;
}
