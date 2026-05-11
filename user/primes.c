#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void sieve(int in_fd[2])
{
    int fd_out[2];
    pipe(fd_out);
    int prime;

    // 读in管道,读完返回关闭
    if (read(in_fd[0], &prime, sizeof(prime)) <= 0)
    {
        close(in_fd[0]);
        exit(0);
    }

    if (prime == -1)
    {
        close(in_fd[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    if (fork() == 0)
    {
        // 不需要写in管道
        close(in_fd[1]);
        sieve(fd_out);
    }

    int num;
    while (read(in_fd[0], &num, sizeof(num)) > 0)
    {
        if (num == -1)
        {
            write(fd_out[1], &num, sizeof(num));
            break;
        }
        // 筛选
        if (num % prime != 0)
        {
            write(fd_out[1], &num, sizeof(num));
        }
    }
    // 写完后关闭
    close(fd_out[1]);
    // 读完关闭
    close(in_fd[0]);
    wait(0);
    exit(0);
}
int main(int argc, char *argv[])
{
    int fd[2];
    pipe(fd);

    //  写入管道,初始化

    if (fork() == 0)
    {
        // child
        // close(fd[1]);
        sieve(fd);
        exit(0);
    }
    else
    {
        for (int i = 2; i <= 35; i++)
        {
            write(fd[1], &i, sizeof(i));
        }
        // 标记写完
        int end = -1;
        write(fd[1], &end, sizeof(1));
        close(fd[1]);
        exit(0);
    }

    exit(0);
}