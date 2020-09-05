#ifndef _DICT_H_
#define _DICT_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5656

#define R 1 //注册
#define L 2 //登录
#define Q 3 //查询
#define H 4 //历史

typedef struct
{
    int type;
    char passwd[32];
    int flag;
    char name[32];
    char data[256];
} MSG;

int client_register(int sockfd, MSG *msg); //注册函数
int client_login(int sockfd, MSG *msg);    //登录函数
int client_query(int sockfd, MSG *msg);    //查询函数
int client_history(int sockfd, MSG *msg);  //查看历史
int root_check(int sockfd, MSG *msg);      //root用户登录
int client_check(int sockfd, MSG *msg);
int root_history(int sockfd, MSG *msg);

#endif
