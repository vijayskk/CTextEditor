#include "../lib/keyprocessing.h"



int main()
{
    
    begin();
    while (1)
    {
        editorWindowRefresh();
        editorProcessKeypress();
    }
    quit();
    return 0;
}
