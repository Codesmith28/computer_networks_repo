#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*explicitly telling that function will not take any parameters*/
void usage(void) {
    printf("Usage\n");
    printf("-n <num> : number of times to display\n");
    printf("-d <char> : character to display\n");
    exit(8);
}

int main(int argc, char *argv[]) {
    // default values:
    int num = 2;
    char disp = '*';
    char c;

    if (argc < 5) {
        printf("Missing arguments\n");
        usage( );
    }

    while (c = getopt(argc, argv, "n:d:"), c != -1) {
        switch (c) {
            case 'n':
                num = atoi(optarg);
                break;
            case 'd':
                disp = optarg[0];
                break;
            default:
                usage( );
                return 1;
        }
    }

    printf("Number of times to display: %d\n", num);
    printf("Character to display: %c\n", disp);

    for (int i = 0; i < num; i++) printf("%c", disp);
    printf("\n");

    return 0;
}
