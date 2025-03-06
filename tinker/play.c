#include <stdio.h>
#include <unistd.h>

int main( ) {
    // get host name:
    char *hostname;
    gethostname(hostname, 1024);
    printf("Host name: %s\n", hostname);
}
