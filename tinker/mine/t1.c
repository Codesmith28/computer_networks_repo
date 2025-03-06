#include <stdio.h>
#include <stdlib.h>

/*
 * argc -> argument count
 * argv -> argument vector
 * */

int main(int argc, char *argv[]) {
    if (argc != 3) { // if not 2 additional arguments provided
        printf("Usage: %s <character> <count>\n", argv[0]);
        return 1;
    }

    char ch = argv[1][0];  // first char of first arg
    int n = atoi(argv[2]); // convert second arg to int

    for (int i = 0; i < n; i++) printf("%c", ch);
    printf("\n");

    return 0;
}
