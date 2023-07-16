# 《TCP IP网络编程》学习笔记

## ch01 理解网络编程和套接字

### 1.1 理解网络编程和套接字

#### 构建 **`接电话`** 套接字（服务器端）

创建服务器端的套接字需要以下函数：

```c
#include <sys/socket.h> 
// 创建套接字成功时返回文件描述符，失败时返回－1。
int socket(int domain, int type, int protocol);
```

```c
#include <sys/socket.h> 
// 绑定成功时返回0，失败时返回-1。
int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
```

```c
#include <sys/socket.h> 
// 将套接字转化为可接收连接的状态，成功时返回0，失败时返回-1。
int listen(int sockfd, int backlog);
```

```c
#include <sys/socket.h> 
// 接受网络连接，成功时返回文件描述符，失败时返回-1。
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

创建服务器端的套接字的基本步骤如下：

- 第一步：调用`socket()`函数创建套接字
- 第二步：调用`bind()`函数绑定套接字的`IP`和 **`端口号`**

- 第三步：调用`listen()`函数把套接字转化为可接收连接状态
- 第四步：调用`accept()`函数接受网络连接

服务器端代码如下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char message[] = "Hello World!";

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // socket()
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // bind()
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // listen()
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    // accept()
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");
    
    // write()
    write(clnt_sock, message, sizeof(message));
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
```

#### 构建 **`打电话`** 套接字（客户端）

创建客户端的套接字需要以下函数：

```c
#include <sys/socket.h> 
// 请求连接，成功时返回文件描述符，失败时返回-1。
int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen);
```

客户端代码如下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // socket()
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
 
    // read()
    str_len = read(sock, message, sizeof(message) - 1);
    if (str_len == -1)
        error_handling("read() error!");

    printf("Message from server : %s \n", message);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
```

#### 在Linux平台下运行

先运行服务器端，等待客户端发送连接：

```shell
rex@rex-virtual-machine:~/tcpip/ch01$ ./hserver 7890

```

启动客户端，向客户端发送连接，此时服务器端接受连接，并向客户端发送`"Hello World!"`，客户端接受到该字符串，并打印其内容：

```shell
rex@rex-virtual-machine:~/tcpip/ch01$ ./hclient 127.0.0.1 7890
Message from server : Hello World!
```

此时发现服务器端和客户端都结束运行：

```shell
rex@rex-virtual-machine:~/tcpip/ch01$ ./hserver 7890
rex@rex-virtual-machine:~/tcpip/ch01$ 
```

### 1.2 基于Linux的文件操作

对于`Linux`而言，文件和`socket`没有区别，但对于Windows而言，两者时不一样的。

>  文件描述符有时也称为文件句柄，但“句柄“主要是Windows中的术语。 因此，本书中如果涉及Windows平台将使用 **`句柄 `** ，如果是Linux平台则用 **`描述符`**。

在`Linux`中，标准输入（`Standard Input`）、标准输出（`Standard Output`）、标准错误（`Standard Error`）默认分别分配了文件描述符`0`、`1`、`2`

文件操作包括打开文件、读取文件、写入文件、关闭文件，分别对应以下函数：

```c
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
// path：文件的路径
// flag：打开文件的模式
// open：打开文件，如果成功就返回该文件的文件描述符，如果失败就返回-1。
int open(const char *path, int flag);
```

文件打开模式如下：

- `O_CREAT`：若文件不存在，创建文件
- `O_TRUNC`：删除文件的内容
- `O_APPEND`：若写入数据，追加到文件尾部
- `O_RDONLY`：只读打开
- `O_WRONLY`：只写打开
- `O_RDWR`：读写打开

```c
#include <unistd.h> 
// fd：文件描述符
// buf：读取文件的缓存地址
// nbytes：读取文件的字节数
// 成功时返回接收的字节数（但遇到文件结尾则返回0) ，失败时返回－1 。
ssize_t read(int fd, void* buf, size_t nbytes);
```

```c
#include <unistd.h> 
// fd：文件描述符
// buf：写入文件的缓存地址
// nbytes：写入文件的字节数
// 成功时返回写入的字节数，失败时返回－1 。
ssize_t write(int fd, const void* buf, size_t nbytes);
```

```c
#include <unistd.h> 
// fd：文件描述符
// 关闭文件描述符
int close(int fd);
```

---

编写一个C语言程序`lower_open_write.c`，向`test.txt`文件写入字符串`"Hello World!\n"`：

```c
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
```

运行结果如下：

```shell
rex@rex-virtual-machine:~/tcpip/ch01$ gcc lower_open_write.c -o low
rex@rex-virtual-machine:~/tcpip/ch01$ ./low 
file descriptor: 3
rex@rex-virtual-machine:~/tcpip/ch01$ cat test.txt 
Hello World!
```

---

编写一个C语言程序`lower_open_read.c`，从`test.txt`中读取内容：

```c
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
```

运行效果如下：

```shell
rex@rex-virtual-machine:~/tcpip/ch01$ gcc lower_open_read.c -o lor
rex@rex-virtual-machine:~/tcpip/ch01$ ./lor 
file descriptor: 3
fild data is Hello World!
```

---

编写一个C语言程序`fd_cmp.c`，分别打开文件和socket，比较其文件描述符：

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    int fd1, fd2, fd3;
    // create a file and two socket
    fd1 = socket(PF_INET, SOCK_STREAM, 0);
    fd2 = open("test.dat", O_CREAT | O_WRONLY | O_TRUNC);
    fd3 = socket(PF_INET, SOCK_DGRAM, 0);
    // output their file descriptor
    printf("file descriptor 1: %d\n", fd1);
    printf("file descriptor 2: %d\n", fd2);
    printf("file descriptor 3: %d\n", fd3);

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
```

运行结果如下：

```shell
rex@rex-virtual-machine:~/tcpip/ch01$ gcc fd_cmp.c -o fc
rex@rex-virtual-machine:~/tcpip/ch01$ ./fc
file descriptor 1: 3
file descriptor 2: 4
file descriptor 3: 5
```

### 1.5 习题

- (1) 套接字在网络编程中的作用是什么？为何称它为套接字？

  - 答：套接字的作用是连接互联网，进行网络通信，`socket`有连接网络的作用，故名套接字。

  - > 套接字在网络编程中扮演着关键的角色，它是实现网络通信的一种机制。套接字提供了一种抽象接口，使应用程序能够通过网络进行数据传输和通信。
    >
    > 套接字可以看作是应用程序与网络之间的“插座”或“接口”，类似于电话插座，它允许应用程序通过网络与其他应用程序或设备进行通信。通过套接字，应用程序可以建立连接、发送和接收数据，并进行网络通信协议的交互。
    >
    > 套接字的名称来源于 Unix 系统的文件抽象概念。在 Unix 中，几乎所有的 I/O 都被视为文件，包括磁盘文件、设备文件以及网络连接。为了提供一致的接口，Unix 将网络连接也抽象为一种特殊类型的文件，称为套接字（socket）。这个术语后来被广泛采用，并成为网络编程中的标准术语。
    >
    > 套接字之所以被称为套接字，是因为它提供了一种“插座”或“接口”的概念，类似于电源插座或电话插座。就像插座允许设备连接到电源或电话线路一样，套接字允许应用程序连接到网络并进行数据传输。套接字提供了一种标准化的接口，使得应用程序可以通过网络进行通信，就像插入插座一样简单。
    >
    > 总结而言，套接字在网络编程中扮演着重要的角色，它是应用程序与网络之间的接口，允许应用程序通过网络进行数据传输和通信。套接字被称为套接字是因为它提供了一种类似于插座的抽象概念，使得应用程序能够连接到网络并进行通信。

- (2) 在服务器端创建套接字后，会依次调用listen函数和accept函数。 请比较并说明二者作用 ？

  - 答：`listen()`函数：将套接字状态转化为可接收连接的状态，成功时，返回0，失败时，返回-1；

  - `accpet()`函数：等待连接，成功时，返回文件描述符，失败时返回-1。

  - > 在服务器端创建套接字后，通常会按照以下顺序调用 `listen()` 函数和 `accept()` 函数。它们的作用如下：
    >
    > 1. `listen()`:
    >    - `listen()` 函数用于将套接字设置为监听状态，以便接受客户端的连接请求。
    >    - 它接受两个参数：套接字描述符和等待连接队列的最大长度。
    >    - 调用 `listen()` 后，套接字开始监听指定的端口，并准备接受传入的连接请求。
    >    - `listen()` 并不会阻塞程序执行，它只是将套接字置于监听状态，等待客户端连接。
    > 2. `accept()`:
    >    - `accept()` 函数用于接受客户端的连接请求，并创建一个新的套接字来处理与该客户端的通信。
    >    - 当有客户端连接请求到达时，`accept()` 会返回一个新的套接字描述符，该描述符用于与客户端进行通信。
    >    - 它接受三个参数：监听套接字描述符、客户端地址结构体指针和客户端地址结构体的长度。
    >    - 调用 `accept()` 会阻塞程序执行，直到有客户端连接请求到达。一旦有连接请求到达，`accept()` 返回并提供一个新的套接字，通过该套接字可以与客户端进行通信。
    >    - 可以使用返回的新套接字进行数据传输和通信，而原始监听套接字仍然保持在监听状态，可以继续接受其他客户端连接请求。
    >
    > 总结：
    >
    > - `listen()` 函数用于将套接字设置为监听状态，开始监听指定的端口，并准备接受传入的连接请求。
    > - `accept()` 函数用于接受客户端的连接请求，并创建一个新的套接字来处理与该客户端的通信。它会阻塞程序执行，直到有客户端连接请求到达，并返回一个新的套接字供与客户端通信使用。
    > - `listen()` 和 `accept()` 通常配合使用，`listen()` 将套接字置于监听状态，而 `accept()` 接受实际的连接请求并创建新的套接字用于通信。这样服务器可以同时处理多个客户端连接请求，并与每个客户端进行独立的通信。

- (3) Linux中，对套接字数据进行`I/O`时可以直接使用文件`I/O`相关函数；而在Windows中则不可以。原因为何？

  - 答：Linux中，文件和socket是一样的，但在Windows上，两者不同。

  - > 在 Linux 中，套接字被视为一种特殊类型的文件描述符，因此可以使用文件 I/O 相关函数（如 `read()` 和 `write()`）直接对套接字进行读写操作。这是因为在 Linux 的设计中，将套接字和其他文件类型统一抽象为文件描述符，使得对它们的操作具有一致性。
    >
    > 然而，在 Windows 中，套接字并不被视为文件描述符，因此不能直接使用文件 I/O 相关函数进行读写操作。Windows 使用了一种不同的模型来处理套接字，称为套接字 API（Socket API），其中包含了专门用于套接字操作的函数，如 `send()` 和 `recv()`。
    >
    > 主要原因是 Windows 和 Linux 在网络编程方面采用了不同的设计理念和实现方式。Windows 套接字 API 提供了更多的功能和灵活性，例如支持异步操作、事件驱动等特性，以满足 Windows 平台上复杂的网络编程需求。相比之下，Linux 选择了简化的设计，将套接字作为文件描述符的一种形式，以提供一致的接口和易用性。
    >
    > 虽然在 Windows 中不能直接使用文件 I/O 相关函数对套接字进行读写操作，但可以使用 Windows 套接字 API 提供的函数来实现类似的功能。这些函数与文件 I/O 函数在用法和参数上可能有所不同，但可以实现相似的功能。
    >
    > 总结：在 Linux 中，套接字被视为文件描述符，可以直接使用文件 I/O 相关函数进行读写操作。而在 Windows 中，套接字不是文件描述符，需要使用专门的套接字 API 函数进行读写操作，这是因为两个操作系统在网络编程方面采用了不同的设计理念和实现方式。

- (4) 创建套接字后一般会给它分配地址，为什么？为了完成地址分配需要调用哪个函数？

  - 答：socket需要指定IP和端口，所以需要分配地址；使用`bind()`函数，给socket绑定IP和端口号。

  - > 在创建套接字后，需要为它分配地址，以便能够在网络中唯一标识和定位该套接字。地址分配是为了确保数据能够正确地发送到目标套接字，并使得其他套接字能够正确地与该套接字进行通信。
    >
    > 在网络编程中，套接字地址通常由 IP 地址和端口号组成。IP 地址用于标识网络中的主机或设备，而端口号用于标识主机上的具体应用程序或服务。通过将套接字与特定的 IP 地址和端口号关联起来，可以确保数据能够准确地传递给目标套接字。
    >
    > 要完成地址分配，需要调用 `bind()` 函数。`bind()` 函数用于将一个套接字与指定的 IP 地址和端口号绑定在一起。它接受三个参数：套接字描述符、指向地址结构体的指针和地址结构体的长度。
    >
    > 在调用 `bind()` 函数时，需要提供一个包含所需 IP 地址和端口号的地址结构体，并将其作为参数传递给 `bind()` 函数。这样，操作系统就会将套接字与指定的地址关联起来，从而完成地址分配。
    >
    > 需要注意的是，在使用 `bind()` 函数之前，需要确保套接字已经被创建并处于可用状态。通常，在创建套接字后，会先设置一些套接字选项（如重用地址选项），然后再调用 `bind()` 函数进行地址分配。
    >
    > 总结：为了在网络中唯一标识和定位套接字，需要为其分配地址，通常由 IP 地址和端口号组成。地址分配可以通过调用 `bind()` 函数来完成，将套接字与指定的地址关联起来。

- (5) Linux中的文件描述符与Windows的句柄实际上非常类似。请以套接字为对象说明它们的含义。

  - 答：

  - > 在Linux和Windows操作系统中，文件描述符和句柄都是用于表示打开的文件或资源的标识符。虽然它们在不同的操作系统中具体实现有所不同，但在功能上非常相似。
    >
    > 在Linux中，文件描述符是一个非负整数，用于唯一标识一个打开的文件、设备或套接字。它是通过调用系统调用（如open()、socket()等）返回的。Linux将文件、设备和套接字都视为文件，并使用文件描述符来引用它们。例如，当你在Linux上创建一个套接字时，系统会返回一个文件描述符，你可以使用该文件描述符进行读取、写入和关闭套接字。
    >
    > 在Windows中，句柄是一个特殊的数据类型，用于标识和访问打开的对象，包括文件、设备和套接字。与Linux的文件描述符类似，句柄也是通过调用相应的函数（如CreateFile()、CreateSocket()等）返回的。对于套接字，Windows使用套接字句柄来进行网络通信操作，比如发送和接收数据。
    >
    > 无论是文件描述符还是句柄，它们都充当了一个抽象层，使应用程序能够通过它们与底层资源进行交互，而无需关心底层实现的细节。通过使用文件描述符或句柄，应用程序可以执行诸如读取、写入、关闭等操作，以及其他与文件或套接字相关的操作。
    >
    > 需要注意的是，尽管文件描述符和句柄在功能上非常相似，但它们的值不能互通。也就是说，你不能将一个文件描述符直接传递给Windows函数，也不能将一个句柄直接传递给Linux函数。这是因为它们在底层实现上有所不同，操作系统使用不同的机制来管理和操作文件描述符和句柄。
    >
    > 总结而言，文件描述符和句柄都是用于标识打开的文件、设备或套接字的标识符。它们提供了一种抽象的方式，使应用程序能够对这些资源进行操作，而无需关心底层实现的细节。

- (6) 底层文件`I/O`函数与ANSI标准定义的`I/O`文件函数之间有何区别？

  - 答：

  - > 底层文件 I/O 函数和 ANSI 标准定义的 I/O 文件函数之间存在一些区别。下面是它们之间的主要区别：
    >
    > 1. 抽象级别：底层文件 I/O 函数提供了对操作系统底层的直接访问，而 ANSI 标准定义的 I/O 文件函数提供了更高级别的抽象。底层文件 I/O 函数通常更接近于操作系统提供的原始功能，而 ANSI 标准定义的 I/O 文件函数则提供了更方便、更易用的接口。
    > 2. 缓冲：ANSI 标准定义的 I/O 文件函数通常使用缓冲来提高性能。例如，stdio.h 中的函数（如fread()、fwrite()）会在内存中使用缓冲区进行读写操作，然后批量地将数据传输到底层文件。这种缓冲机制可以显著提高 I/O 性能。而底层文件 I/O 函数通常不提供缓冲功能，需要手动实现。
    > 3. 可移植性：ANSI 标准定义的 I/O 文件函数是跨平台的，可以在不同的操作系统上使用相同的代码。这些函数已经被广泛实现和标准化，因此具有较好的可移植性。而底层文件 I/O 函数在不同的操作系统上可能有所不同，需要根据具体的操作系统进行适配。
    > 4. 错误处理：ANSI 标准定义的 I/O 文件函数通常使用返回值来指示错误。例如，当读取文件时，可以通过返回值来判断是否读取成功。而底层文件 I/O 函数通常使用错误码或异常机制来处理错误。这些函数可能会返回特定的错误码，或者抛出异常供应用程序捕获和处理。
    > 5. 功能扩展：ANSI 标准定义的 I/O 文件函数提供了一系列功能丰富的函数，如格式化输入输出、文件定位等。这些函数使得对文件的操作更加方便和灵活。而底层文件 I/O 函数通常只提供基本的读写功能，其他高级功能需要手动实现。
    >
    > 总的来说，底层文件 I/O 函数提供了对操作系统底层的直接访问，更接近于操作系统提供的原始功能，但使用起来相对较复杂。而 ANSI 标准定义的 I/O 文件函数提供了更高级别的抽象，更易用、可移植，并提供了丰富的功能扩展。选择使用哪种函数取决于具体的需求和平台要求。

- 参考本书给出的示例`low—open.c`和`low_read.c`，分别利用底层文件`I/O`和ANSI标准`I/O`编写文件复制程序。可任意指定复制程序的使用方法。

  - 使用底层文件`I/O`编写的文件复制程序如下：

  - ```c
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
    ```

  - 运行结果如下：

  - ```shell
    rex@rex-virtual-machine:~/tcpip/ch01$ gcc lower_copy.c -o lc
    rex@rex-virtual-machine:~/tcpip/ch01$ cat file1.txt 
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    rex@rex-virtual-machine:~/tcpip/ch01$ ./lc file1.txt file2.txt 
    rex@rex-virtual-machine:~/tcpip/ch01$ cat file2.txt 
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    ```

  - 使用底层文件`I/O`编写的文件复制程序如下：

  - ```c
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
    ```

  - 运行结果如下：

  - ```shell
    rex@rex-virtual-machine:~/tcpip/ch01$ gcc ansi_copy.c -o ac
    rex@rex-virtual-machine:~/tcpip/ch01$ rm file2.txt 
    rex@rex-virtual-machine:~/tcpip/ch01$ ./ac file1.txt file2.txt 
    rex@rex-virtual-machine:~/tcpip/ch01$ cat file2.txt 
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    Studying In SZU.
    ```

### 以`_t`为后缀的数据类型

> - 我们已经接触到`ssize_t` 、 `size_t`等陌生的数据类型。这些都是元数据类型 (`primitive`)，在`sys/types.h`头文件中一般由`typedef`声明定义，算是给大家熟悉的基本数据类型起了别名。
>
> - 既然已经有了基本数据类型，为何还要声明并使用这些新的呢？人们目前普遍认为`int`是32位的，因为主流操作系统和计算机仍采用32位。而在过去
>   16位操作系统时代，int类型是16位的。 根据系统的不同、时代的变化，数据类型的表现形式也随之改变，需要修改程序中使用的数据类型 。
> - 如果之前已在需要声明4字节数据类型之处使用了`size_t`或`ssize_t`， 则将大大减少代码变动，因为只需要修改并编译`size_t`和`ssize_t`的`typedef`声明即可。在项目中，为了给基本数据类型赋予别名，一般会添加大量`typedef`声明 。而为了与程序员定义的新数据类型加以区分，操作系统定义的数据类型会添加后缀`＿t` 。
