#include "sui.hpp"
#include <stdio.h>

void printr(SuiRect rect)
{
        printf("rect(%i, %i, %i, %i)\n", rect.x, rect.y, rect.w, rect.h);
}

int main()
{
        printr(SuiRect(1, 2, 3, 5));
        return 0;
}