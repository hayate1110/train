#include <stdio.h>

/* データセットを意識して読み込む */

int main(void)
{
    int count;
    char buf[64];

    while (1) {
        /* データセットの先頭（データ数）を読む */
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            break; /* データセットの異常終了 */
        }
        sscanf(buf, "%d", &count); /* データセット中のデータ数 */
        printf("%d\n", count); /* データ数の書き出し */

        if (count == 0) { /*.データ全体の終了の判定 */
            break;
        }

        /* データセット（列車データ）を読み込む */
        while (count-- > 0) {
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                break; /* データセットの異常終了 */
            }

            /* printf("%s", buf); */ /* 列車データの書き出し */
        }
    }
}