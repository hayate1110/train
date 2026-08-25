#include <stdio.h>

/* 各行ごとに読み込んで、そのまま出力する */

int main(void)
{
    char buf[64];

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        fputs(buf, stdout);
    }
}