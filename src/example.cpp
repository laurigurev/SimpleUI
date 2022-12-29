#include "sui.hpp"
#include <stdio.h>

void printcmdr(const SuiCommandRect& cmdrect)
{
        printf("SuiCommandRect({%i, %i, %i, %i}, {%u, %u, %u, %u})\n", cmdrect.rect.x, cmdrect.rect.y, cmdrect.rect.w, cmdrect.rect.h, cmdrect.color.r,
               cmdrect.color.g, cmdrect.color.b, cmdrect.color.a);
}

int main()
{
        SuiContext sui;
        sui.begin("window", SuiRect(50, 50, 100, 100));
        sui.rect();
        sui.end();
        printcmdr(sui.cmdrects.get());
        return 0;
}