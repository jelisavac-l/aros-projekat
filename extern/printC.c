#include <stdio.h>
#include "printC.h"

void printc(char *output, char* color)
{
    printf("%s%s%s", color, output, CRESET);
}
