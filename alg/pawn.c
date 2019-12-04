#include <stdio.h>
#include <stdlib.h>

/*
 * return 0, for failed
 */
int solution(int A[], int N)
{
    int *count =calloc(N, sizeof(int));
    if (!count) {
        return 0;
    }

    int i = 0;
    do {
        count[i]++;
        //printf("%d = %d\n", i, count[i]);
        /*
         * loop now
         */
        if (count[i] == 3) {
            break;
        }
        i = A[i];
    } while (1);

    int cycle = 0;
    for (i = 0; i < N; i++) {
        if (count[i] >= 2) {
            cycle++;
        }
    }

    return cycle;
}


int main(int argc, char *argv[])
{
    int test[] = {2, 3, 1, 1, 3};
    printf("cycle = %d\n", solution(test, 5));
}
