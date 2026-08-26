#include <stdio.h>
#include <string.h>

#define MAXCITY 100

/* 駅名の表 */
char city_name[MAXCITY][18];
int ncity; /* 駅数 */

#define MAXCONN 2000+1

/* 列車の情報 */
struct train {
    int from, to; /* 駅番号 */
    int dpt, arv; /* 0:00 からの分単位 */
    int fare;
} trains[MAXCONN];

int nconn; /* 列車数 */

void parse_connection(char *buf);
int city_id(char *name);

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
            parse_connection(buf);
            /* printf("%s", buf); */ /* 列車データの書き出し */
        }
    }
}

/* 列車データの解析 */
void parse_connection(char *buf)
{
    char from[18], to[18];
    int dpt[2], arv[2], fare;
    
    sscanf(buf, "%s %d:%d %s %d:%d %d", 
        from, &(dpt[0]), &(dpt[1]), to, &(arv[0]), (&arv[1]), &fare);
    
    trains[nconn].from = city_id(from);
    trains[nconn].to = city_id(to);
    trains[nconn].dpt = dpt[0]*60+dpt[1];
    trains[nconn].fare = fare;
    nconn++;
}

/* 駅名 -> 駅番号 */
int city_id(char *name)
{
    int i;

    for (i = 0; i < ncity ; i++) {
        if (strcmp(name, &(city_name[i][0])) == 0) {
            return i;
        }
    }

    strcpy(&(city_name[ncity][0]), name);
    return ncity++;
}