#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


int main(int argc, char *argv[])
{ 
    int fdf2c[2];
    int fdc2f[2];

    int pid;
    if(pipe(fdf2c) == -1) {
        printf("pipe father to child error\n");
        exit(1);
    }

    if(pipe(fdc2f) == -1) {
        printf("pipe father to child error\n");
        exit(1);
    }

    pid = fork();

    if(pid < 0){
        printf("fork error\n");
        exit(1);
    }

    if(pid > 0) {
        write(fdf2c[1],"a", 1);

        char buf[1];
        read(fdc2f[0], buf, sizeof(buf));
        printf("%d: receied pong\n", getpid());
        wait(0);
    }else{
        char buf[1];
        read(fdf2c[0], buf, sizeof(buf));
        printf("%d: revice ping\n", getpid());
        write(fdc2f[1], &buf[0], 1);
    }

    exit(0);

}