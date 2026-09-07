#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    printf("st1 = %s\n", st1);
    printf("st2 = %s\n", st2);
    printf("dpt = %02d:%02d\n", dpt_h, dpt_m);
    printf("arv = %02d:%02d\n", arv_h, arv_m);
    printf("stay_minimum = %d\n", stay_minimum);

    return 0;
}