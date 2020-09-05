#include "server_dict.h"

int main()
{
  int sockfd;
  struct sockaddr_in linkfd;
  char *errmsg;
  sqlite3 *db;

  if (sqlite3_open(DATABASE, &db) != SQLITE_OK)
  {
    printf("%s\n", sqlite3_errmsg(db));
    return -1;
  }

  if (sqlite3_exec(
          db,
          "create table if not exists user(name text primary key,passwd text);",
          NULL, NULL, &errmsg) != SQLITE_OK)
  {
    printf("%s\n", errmsg);
  }
  else
  {
    printf("user table create or open table success.\n");
  }

  if (sqlite3_exec(
          db,
          "create table if not exists record(name text,date text,word text);",
          NULL, NULL, &errmsg) != SQLITE_OK)
  {
    printf("%s\n", errmsg);
  }
  else
  {
    printf("record table create or open table success.\n");
  }

  if (sqlite3_exec(db, "insert into user values('root','1');", NULL, NULL,
                   &errmsg) != SQLITE_OK)
  {
    printf("%s\n", errmsg);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("failed socket");
    return -1;
  }

  int b_reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof(b_reuse));

  bzero(&linkfd, sizeof(linkfd));
  linkfd.sin_family = AF_INET;
  linkfd.sin_port = htons(SERVER_PORT);
  linkfd.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sockfd, (struct sockaddr *)&linkfd, sizeof(linkfd)) < 0)
  {
    perror("failed bind\n");
    return -1;
  }

  if (listen(sockfd, 5) < 0)
  {
    perror("failed listen");
    return -1;
  }

  printf("server start:\n");
  int acceptfd;
  pid_t pid;

  signal(SIGCHLD, SIG_IGN);

  for (;;)
  {
    if ((acceptfd = accept(sockfd, NULL, NULL)) < 0)
    {
      perror("failed accept");
      return -1;
    }

    if ((pid = fork()) < 0)
    {
      perror("failed fork");
      return -1;
    }
    else if (pid == 0)
    {
      close(sockfd);
      do_client(acceptfd, db);
    }
    else
    {
      close(acceptfd);
    }
  }
  return 0;
}
