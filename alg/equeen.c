#include <stdio.h>

#define N   8

int test_pos(int *positions, int row, int column)
{
    for (int i = 0; i < row; i++) {
        if ((positions[i] == column) ||
            ((positions[i] + (row - i)) == column) ||
            ((positions[i] - (row - i)) == column))
            return 0;
    }

    return 1;
}

void print_pos(int *positions)
{
    static int index = 0;
    printf("Solution %d:\n", ++index);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (positions[i] == j)
                printf("X ");
            else
                printf("- ");
        }
        printf("\n");
    }
    printf("\n");
}

void place(int *positions, int row)
{
    /* Done */
    if (row == N) {
        print_pos(positions);
        return;
    }

    for(int column = 0; column < N; column++) {
        if (test_pos(positions, row, column)) {
            positions[row] = column;
            place(positions, row + 1);
        }
    }
}

int main(int argc, char **argv)
{
    int positions[N];
    place(positions, 0);
}
