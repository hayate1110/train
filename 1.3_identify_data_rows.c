#include <stdio.h>

/* データ数の行と列車データの行を判別する */

int main(void)
{
    int count;
    char buf[64];

    count = 0;
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (count == 0) { /* データ数の行 */
            sscanf(buf, "%i", &count);
            printf("%i\n", count);
        }
        else { /* 列車データの行 */
            /* fputs(buf, stdout); */
            count--;
        }
    }
}