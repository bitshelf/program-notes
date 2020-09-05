#ifndef __FTP_H_
#define __FTP_H_

#include <arpa/inet.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>

//最大信息传递
#define MAX 1024
#define SER_PORT 8866
#define SER_IP "127.0.0.1"

//定义通信类型
typedef struct
{
  int type;
  char data[MAX];
} MSG;

typedef struct
{
  int acceptfd;
  MSG *msg;
  char file_name[128];
} thread_arg;

#define L 1 //客户端查看当前目录下文件
#define P 2 //客户端上传文件，服务器拉取文件
#define G 3 //客户端拉取文件，服务器发送文件到客户端

int pullfile(int acceptfd, MSG *msg);
int pushfile(int acceptfd, MSG *msg);
// int filewirte(int acceptfd, MSG *msg, char const filename[]);
static void *filewrite(void *args);
int filesend(int acceptfd, MSG *msg, const char filename[]);
void help();
int fileshow();
int client_listfile(int acceptfd, MSG *msg);

#endif
