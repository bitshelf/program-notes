#ifndef __FTP_H_
#define __FTP_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>

//最大信息传递
#define MAX 1024
#define SER_PORT 8866

//定义通信类型
typedef struct
{
    int type;
    char data[MAX];
} MSG;

#define L 1 //客户端查看当前目录下文件
#define P 2 //客户端上传文件，服务器拉取文件
#define G 3 //客户端拉取文件，服务器发送文件到客户端

int pullfile(int acceptfd, MSG *msg);
int pushfile(int acceptfd, MSG *msg);
int showdir(int acceptfd, MSG *msg);
int listfile(int acceptfd, MSG *msg);
int filewirte(int acceptfd, MSG *msg, char const filename[]);
int filesend(int acceptfd, MSG *msg, const char filename[]);
void help();
int fileshow();
int client_listfile(int acceptfd, MSG *msg);
int ser_send(int acceptfd, MSG *msg);
int ser_get(int acceptfd, MSG *msg);
int do_client(int acceptfd);
size_t readn(int acceptfd, void *buffer, size_t size);

#endif
