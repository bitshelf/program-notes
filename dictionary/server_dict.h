#ifndef _DICT_H_
#define _DICT_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <sqlite3.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#define SERVER_PORT 5656

#define DATABASE "sql.db"

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

int ser_register(int acceptfd, MSG *msg, sqlite3 *db);
int ser_login(int acceptfd, MSG *msg, sqlite3 *db);
int ser_query(int acceptfd, MSG *msg, sqlite3 *db);
int get_history(int acceptfd, MSG *msg, sqlite3 *db);
int do_client(int acceptfd, sqlite3 *db);
int server_searchword(int acceptfd, MSG *msg, const char word[]);
int get_time(char *date);
int ser_findword(int acceptfd, MSG *msg, sqlite3 *db);
int history_show(void *arg, int f_num, char **f_value, char **f_name);
int call_history(int acceptfd, MSG *msg, sqlite3 *db);

#endif
