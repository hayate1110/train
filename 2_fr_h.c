#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

#define MAXCITY 100

/* 駅名の表 */
char city_name[MAXCITY][18];
int ncity; /* 駅数 */

# define MAXCONN 2000+1
/* 列車の情報 */
struct train {
    int from, to; /* 駅番号 */
    int dpt, arv; /* 0:00 からの分単位 */
    int fare;
} trains[MAXCONN];

int nconn; /* 列車数 */

#define INFINITE 99999999

int from_hakodate[MAXCITY][MAXCONN], /* fr_h */
    from_tokyo[MAXCITY][MAXCONN]; /* fr_t */

void parse_connection(char *buf);
int city_id(char *name);
int cmp_arv(const void *t1, const void *t2);
void prepare_data(void);
int change(struct train tv[], int p, int st, int dpttime);
void make_table(int v[MAXCITY][MAXCONN], int org, struct train tv[]);
void show_table(int v[MAXCITY][MAXCONN], int org, struct train trains[]);

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

        if (count == 0) { /*.データ全体の終了の判定 */
            break;
        }

        /* データセット（列車データ）を読み込む */
        while (count-- > 0) {
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                break; /* データセットの異常終了 */
            }
            parse_connection(buf);
        }
        prepare_data(); /* 列車データの準備 */
        make_table(from_hakodate, city_id("Hakodate"), trains); /* fr_h の作成 */
        make_table(from_tokyo, city_id("Tokyo"), trains); /* fr_t の作成 */

        printf("from Hakodate\n");
        show_table(from_hakodate, city_id("Hakodate"), trains); /* fr_h の表示 */

        printf("from Tokyo\n");
        show_table(from_tokyo, city_id("Tokyo"), trains); /* fr_t の表示 */
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

/* 列車到着時刻の大小比較関数の定義 */
int cmp_arv(const void *t1, const void *t2)
{
    const struct train *train1 = t1;
    const struct train *train2 = t2;
    return train1->arv - train2->arv;
}

void prepare_data(void)
{
    /* 到着時刻の早い順に列車の配列の要素を並べ替え */
    qsort(/* 並べ替え対象の配列 */ trains,
          /* 配列の要素数 */ nconn,
          /* train型構造体の大きさ */ sizeof(struct train),
          /* 並べ替えで用いる大小比較関数 */ cmp_arv);
}

/* 列車の配列tv中で、dpttime以前に駅stに到着する列車を探す
    p: 探索を開始する列車到着事象の番号 */
int change(struct train tv[], int p, int st, int dpttime)
{
    while (p >= 0) {
        if ((tv[p].to == st) && 
            (tv[p].arv <= dpttime)) break;
        p--;
    }
    return p;
}

/* 列車の配列tvを参照して、駅orgを起点として、
    fr_h または fr_t を二次元配列vに作成する  */
void make_table(int v[MAXCITY][MAXCONN], int org, struct train tv[])
{
    int ti;
    int i;
    int a;

    for (i = 0; i < ncity ; i++) {
        v[i][0] = INFINITE;         /* 式(1) 2行目 */
    }
    v[org][0] = 0;          /* 式(1) 1行目 */

    /* 列車の到着時刻の早い順に繰り返し */
    for (ti = 0; ti < nconn ; ti++) {
        for (i = 0; i < ncity ; i++) {
            v[i][ti+1] = v[i][ti];  /* 式(1) 3行目 */
        }

        /* 列車の配列tv中で、tv[ti].dpt以前に駅tv[ti].fromに到着する列車を探す 
            = 列車tv[ti]に乗り継げる列車を探す
            ti-1: 探索を開始する列車到着事象の番号*/
        a = change(tv, ti-1, tv[ti].from, tv[ti].dpt);

        /* 式(1) 4行目 */
        v[tv[ti].to][ti+1] = min(v[tv[ti].to][ti], 
                                 tv[ti].fare + v[tv[ti].from][a+1]);

    }
}

void show_table(int v[MAXCITY][MAXCONN], int org, struct train trains[])
{
    int ti, i;

    printf("| arrival ");
    for (i = 0; i < ncity ; i++) {
        printf("| %s ", &(city_name[i][0]));
    }
    printf("\n");

    printf("| 8:00 ");
    for (i = 0; i < ncity ; i++) {
        if (v[i][0] == INFINITE) {
            printf("| INF ");
        } else {
            printf("| %d ", v[i][0]);
        }
    }
    printf("\n");

    for (ti = 0; ti < nconn ; ti++) {
        printf("| %d:%02d ", trains[ti].arv/60, trains[ti].arv%60);
        for (i = 0; i < ncity ; i++) {
            if (v[i][ti+1] == INFINITE) {
                printf("| INF ");
            } else {
                printf("| %d ", v[i][ti+1]);
            }
        }
        printf("\n");
     }
}