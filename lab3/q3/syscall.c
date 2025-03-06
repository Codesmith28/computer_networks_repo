#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main( ) {
    pid_t pid1, pid2;

    pid1 = fork( );
    if (pid1 == 0) {
        printf("Child 1 PID: %d\n", getpid( ));
        printf("Child 1 Parent PID: %d\n", getppid( ));
        wait(NULL);
        execv("/bin/date", NULL);
    } else {
        pid2 = fork( );
        if (pid2 == 0) {
            printf("Child 2 PID: %d\n", getpid( ));
            printf("Child 2 Parent PID: %d\n", getppid( ));
            wait(NULL);
            execv("/bin/ls", NULL);
        } else {
            printf("Parent PID: %d\n", getpid( ));
            printf("Parent Parent PID: %d\n", getppid( ));
        }
    }
}
