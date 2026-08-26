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
void print_most_expensive_train(struct train trains[], int nconn);
void print_longest_train(struct train trains[], int nconn);
void print_departing_trains(struct train trains[], int nconn, int city);
void print_arriving_trains(struct train trains[], int n, int city);

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
        /* print_most_expensive_train(trains, nconn); */
        /* print_longest_train(trains, nconn); */
        /* print_departing_trains(trains, nconn, 1); */
        print_arriving_trains(trains, nconn, 1);
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
    trains[nconn].arv = arv[0]*60+arv[1];
    trains[nconn].fare = fare;
    nconn++;
}

/* 駅名 -> 駅番号 */
int city_id(char *name)
{
    int i;

    for (i = 0 ; i < ncity ; i++) {
        if (strcmp(name, &(city_name[i][0])) == 0) {
            return i;
        }
    }

    strcpy(&(city_name[ncity][0]), name);
    return ncity++;
}

/* 最も金額が高い列車を出力する */
void print_most_expensive_train(struct train trains[], int nconn)
{
    int max = 0;

    for (int i = 0 ; i < nconn ; i++) {
        if (trains[i].fare > trains[max].fare) {
            max = i;
        }
    }

    printf("%d\n", trains[max].fare);
}

/* 最も時間がかかる列車を求める（出力する） */
void print_longest_train(struct train trains[], int nconn)
{
    int longest = 0;

    for (int i = 0 ; i < nconn ; i++) {
        if (trains[i].arv - trains[i].dpt > trains[longest].arv - trains[longest].dpt) {
            longest = i;
        }
    }

    printf("%d\n", trains[longest].arv - trains[longest].dpt);
}

void print_departing_trains(struct train trains[], int nconn, int city)
{
    for (int i = 0 ; i < nconn ; i++) {
        if (trains[i].from == city) {
            printf("%s %d %s %d %d\n", &(city_name[trains[i].from][0]), trains[i].dpt, &(city_name[trains[i].to][0]), trains[i].arv, trains[i].fare);
        }
    }
}

void print_arriving_trains(struct train trains[], int n, int city)
{
    for (int i = 0 ; i < nconn ; i++) {
        if (trains[i].to == city) {
            printf("%s %d %s %d %d\n", &(city_name[trains[i].from][0]), trains[i].dpt, &(city_name[trains[i].to][0]), trains[i].arv, trains[i].fare);
        }
    }
}