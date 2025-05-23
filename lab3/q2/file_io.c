#include <stdio.h>
#include <stdlib.h>

int main( ) {
    /* Pointer to the source file */
    FILE *src;
    /* File is read one character at a time*/
    char c;
    /* Opening source file in read mode*/

    src = fopen("sample.txt", "r");
    if (src == NULL) {
        printf("File not found. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    /* Read src until end-of-file char is encountered */
    while ((c = fgetc(src)) != EOF) {
        printf("%c", c);
    }

    fclose(src);

    return 0;
}
