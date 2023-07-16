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

    FILE *fd1, *fd2;
    char buf[BUF_SIZE];

    fd1 = fopen(argv[1], "r");
    fd2 = fopen(argv[2], "w");

    if (fd1 == NULL || fd2 == NULL)
        error_handling("fopen() error!");

    while (1)
    {   
        size_t num_rd;
        num_rd = fread(buf, 1, sizeof buf, fd1);
        if (num_rd == -1)
        {
            error_handling("fread() error!");
        }
        else if (num_rd == 0)
        {
            break;
        }

        size_t num_wr;
        num_wr = fwrite(buf, 1, num_rd, fd2);

        if (num_wr == -1)
        {
            error_handling("fwrite() error!");
        }
    }   
}

void error_handling(char* msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}