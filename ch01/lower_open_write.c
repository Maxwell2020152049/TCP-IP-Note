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
    char buf[BUF_SIZE] = "Hello World!\n";

    fd = open("test.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if (fd == -1)
        error_handling("open() error!");

    printf("file descriptor: %d\n", fd);
    
    if (write(fd, buf, sizeof buf) == -1)
        error_handling("write() error!");

    close(fd);

    return 0;
}