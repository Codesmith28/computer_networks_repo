#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    /*read arguments */
    char a;
    /*store filename in a variable*/
    char *ip_file = NULL;
    char *op_file = NULL;

    while (a = getopt(argc, argv, "i:o:"), a != -1) {
        switch (a) {
            case 'i':
                ip_file = optarg;
                break;
            case 'o':
                op_file = optarg;
                break;
            default:
                printf("invalid flag!\n");
                break;
        }
    }

    if (ip_file == NULL) {
        printf("No ip_file provided. Using default ip_file.\n");
        ip_file = "./sample.txt";
    }

    if (op_file == NULL) {
        printf("No op_file provided. Using default op_file.\n");
        op_file = "./sample_output.txt";
    }

    /* Pointer to the source and destination file */
    FILE *src;
    FILE *dest;

    /* File is read one character at a time*/
    char c;

    /* Opening source file in read mode*/
    src = fopen(ip_file, "r");
    if (src == NULL) {
        printf("File not found. Exiting.\n");
        return 1;
    }

    dest = fopen(op_file, "w");
    if (dest == NULL) {
        printf("File not found. Exiting.\n");
        return 1;
    }

    /* Read src until end-of-file char is encountered */
    while ((c = fgetc(src)) != EOF) {
        fprintf(dest, "%c", c);
    }

    fclose(src);
    fclose(dest);
    return 0;
}
