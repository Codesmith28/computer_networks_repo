#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    /*read arguments */
    char a;
    /*store filename in a variable*/
    char *filename = NULL;

    while (a = getopt(argc, argv, "i:"), a != -1) {
        switch (a) {
            case 'i':
                filename = optarg;
                break;
            default:
                printf("invalid flag!\n");
                break;
        }
    }

    if (filename == NULL) {
        printf("No filename provided. Using default filename.\n");
        filename = "./sample.txt";
    }

    /* Pointer to the source file */
    FILE *src;
    /* File is read one character at a time*/
    char c;

    /* Opening source file in read mode*/
    src = fopen(filename, "r");
    if (src == NULL) {
        printf("File not found. Exiting.\n");
        return 1;
    }

    /* Read src until end-of-file char is encountered */
    while ((c = fgetc(src)) != EOF) {
        printf("%c", c);
    }

    fclose(src);
    return 0;
}
