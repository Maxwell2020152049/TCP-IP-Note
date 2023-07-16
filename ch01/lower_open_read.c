#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUF_SIZE 128

void error_handling(char* msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

int main()
{
    int fd;
    char buf[BUF_SIZE];

    fd = open("test.txt", O_CREAT | O_RDONLY);

    if (fd == -1)
        error_handling("open() error!");

    printf("file descriptor: %d\n", fd);
    
    if (read(fd, buf, sizeof buf) == -1)
        error_handling("read() error!");

    printf("fild data is %s\n", buf);

    close(fd);

    return 0;
}