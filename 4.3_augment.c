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
} trains[MAXCONN], rtrains[MAXCONN];

int nconn; /* 列車数 */

#define BIAS 24*60 /* 24h*60m */

#define INFINITE 99999999

int from_st1[MAXCITY][MAXCONN], /* fr_h */
    from_st2[MAXCITY][MAXCONN], /* fr_t */
    to_st1[MAXCITY][MAXCONN], /* to_h */
    to_st2[MAXCITY][MAXCONN]; /* to_t */

void parse_connection(char *buf, int dpt_time_limit, int arv_time_limit);
int city_id(char *name);
int cmp_arv(const void *t1, const void *t2);
void prepare_data(void);
int change(struct train tv[], int p, int st, int dpttime);
void make_table(int v[MAXCITY][MAXCONN], int org, struct train tv[]);
int calc_cost(int city, int stay_minimum);
void solve(char *st1, char *st2, int stay_minimum);

/* データセットを意識して読み込む */

int main(int argc, char *argv[])
{
    char *st1 = NULL;
    char *st2 = NULL;
    int dpt_h, dpt_m;
    int arv_h, arv_m;
    int stay_minimum;

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-st1") == 0) {
            st1 = argv[++i];

        } else if (strcmp(argv[i], "-st2") == 0) {
            st2 = argv[++i];

        } else if (strcmp(argv[i], "-dpt") == 0) {
            sscanf(argv[++i], "%d:%d", &dpt_h, &dpt_m);

        } else if (strcmp(argv[i], "-arv") == 0) {
            sscanf(argv[++i], "%d:%d", &arv_h, &arv_m);

        } else if (strcmp(argv[i], "-stay_minimum") == 0) {
            stay_minimum = atoi(argv[++i]);

        } else {
            fprintf(stderr, "usage: %s -st1 <string> -st2 <string> -dpt <int>:<int> -arv <int>:<int> -stay_minimum <int>\n", argv[0]);
            return 1;
        }
    }

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

        ncity = 0;
        nconn = 0;

        /* データセット（列車データ）を読み込む */
        while (count-- > 0) {
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                break; /* データセットの異常終了 */
            }
            parse_connection(buf, dpt_h * 60 + dpt_m, arv_h * 60 + arv_m);
        }
        solve(st1, st2, stay_minimum);
        printf("\n");
    }
}

/* 列車データの解析 */
void parse_connection(char *buf, int dpt_time_limit, int arv_time_limit)
{
    char from[18], to[18];
    int dpt[2], arv[2], fare;
    
    sscanf(buf, "%s %d:%d %s %d:%d %d", 
        from, &(dpt[0]), &(dpt[1]), to, &(arv[0]), (&arv[1]), &fare);

    /* 8:00以前の発車、18:00以降の到着は除外 */
    if (dpt[0] * 60 + dpt[1] < dpt_time_limit || arv[0] * 60 + arv[1] > arv_time_limit) {
        return;
    }

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
    int i;

    for (i=0; i<nconn; i++) {
        rtrains[i].from = trains[i].to;
        rtrains[i].to = trains[i].from;
        rtrains[i].dpt = BIAS - trains[i].arv;
        rtrains[i].arv = BIAS - trains[i].dpt;
        rtrains[i].fare = trains[i].fare;
    }

    /* 到着時刻の早い順に列車の配列の要素を並べ替え */
    qsort(/* 並べ替え対象の配列 */ trains,
          /* 配列の要素数 */ nconn,
          /* train型構造体の大きさ */ sizeof(struct train),
          /* 並べ替えで用いる大小比較関数 */ cmp_arv);

    /* 到着時刻の早い順に列車の配列の要素を並べ替え */
    qsort(/* 並べ替え対象の配列 */ rtrains,
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

int calc_cost(int city, int stay_minimum)
{
    int a=0, d=nconn-1;
    int stay;
    int c, min_c=INFINITE;

    while(1) {
        stay = (BIAS - rtrains[d].arv) - trains[a].arv;
        if (stay < stay_minimum) {
            d--;
            if (d < 0) {
                break;
            }
            continue;
        }
        
        c = from_st1[city][a+1] + 
            from_st2[city][a+1] + 
            to_st1[city][d+1] + 
            to_st2[city][d+1];
        
        if (c < min_c) {
            min_c = c;
        }
        a++; /* 駅stへの到着時刻を後ろへずらす */
        if (a >= nconn) {
            break;
        }
    }
    return min_c;
}

void solve(char *st1, char *st2, int stay_minimum)
{
    int c, a_i;
    int cost, min_cost;

    prepare_data();
    make_table(from_st1, city_id(st1), trains);
    make_table(from_st2, city_id(st2), trains);
    make_table(to_st1, city_id(st1), rtrains);
    make_table(to_st2, city_id(st2), rtrains);

    min_cost = INFINITE;
    for (c=0; c < ncity; c++) {
        cost = calc_cost(c, stay_minimum);
        printf("%s: %d\n", &(city_name[c][0]), cost);
    }
    printf("\n");
}