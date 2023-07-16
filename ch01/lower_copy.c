#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUF_SIZE 128

void error_handling(char* msg);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        error_handling("arguments number error!");
    }

    int fd1, fd2;
    char buf[BUF_SIZE];

    fd1 = open(argv[1], O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    fd2 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd1 == -1 || fd2 == -1)
        error_handling("open() error!");

    while (1)
    {   
        int num_rd;
        num_rd = read(fd1, buf, sizeof buf);
        if (num_rd == -1)
        {
            error_handling("read() error!");
        }
        else if (num_rd == 0)
        {
            break;
        }

        int num_wr;
        num_wr = write(fd2, buf, num_rd);

        if (num_wr == -1)
        {
            error_handling("write() error!");
        }
    }   
}

void error_handling(char* msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}