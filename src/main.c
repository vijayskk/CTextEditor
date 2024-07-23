#include "../lib/keyprocessing.h"



int main(int argc, char const *argv[])
{
    
    begin();

    if (argc >= 2)
    {
        editorOpen(argv[1]);
    }

    while (1)
    {
        editorWindowRefresh();
        editorProcessKeypress();
    }
    quit();
    return 0;
}
