#include "client_fun.h"

//客户端拉取服务区的文件
int pullfile(int acceptfd, MSG *msg)
{
  char file_name[128];
  FILE *fp;

  printf("服务器当前目录下有以下文件：\n");
  client_listfile(acceptfd, msg);
  printf("请选择文件:\n");
  msg->type = G;
  send(acceptfd, msg, sizeof(MSG), 0);

  scanf("%s", file_name);
  strcpy(msg->data, file_name);

  //发送文件名,用于打开客户端文件
  if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed send to filename");
    return 0;
  }
  thread_arg args;
  args.acceptfd = acceptfd;
  strcpy(args.file_name, file_name);
  args.msg = msg;
  pthread_t ptid;
  pthread_create(&ptid, NULL, filewrite, (void *)&args);
  pthread_join(ptid,NULL);
  // filewrite(acceptfd, msg, file_name); //j接收文件
}

//客户端选择文件发送
int pushfile(int acceptfd, MSG *msg)
{
  msg->type = 2;
  char file_name[128];
  if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed push");
    return -1;
  }

  printf("当前目录有以下文件：\n");
  fileshow();
  printf("请选择要传输的文件：");
  scanf("%s", file_name);

  strcpy(msg->data, file_name);
  send(acceptfd, msg, sizeof(MSG), 0); //发送文件名，用于创建文件
  filesend(acceptfd, msg, file_name);  //长传文件
  return 0;
}

//将文件通过网络发送
int filesend(int acceptfd, MSG *msg, const char filename[])
{
  FILE *fp;
  long lSize;
  char *buffer;
  size_t result;

  if ((fp = fopen(filename, "rb")) == NULL)
  {
    perror("failed open file");
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  lSize = ftell(fp); //获取文件大小，用于开辟给对方开辟缓冲区
  rewind(fp);

  //发送文件大小
  sprintf(msg->data, "%ld", lSize);
  if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed send to file size");
    return 0;
  }

  //开辟缓冲区
  buffer = (char *)malloc(sizeof(char) * lSize);
  if (buffer == NULL)
  {
    perror("failed malloc");
    exit(1);
  }

  //读入文件
  result = fread(buffer, sizeof(char), lSize, fp);
  if (result != lSize)
  {
    perror("failed fread");
    exit(1);
  }

  //发送文件
  const char *cp;
  cp = buffer;
  while (lSize)
  {
    int n_written = send(acceptfd, cp, lSize, 0);
    if (n_written <= 0)
    {
      error(1, errno, "send failed");
      return -1;
    }
    lSize -= n_written;
    cp += n_written;
  }

  printf("上传完毕!\n");
  free(buffer);
  fclose(fp);
  return 0;
}

//将接收的文件写入当前目录下
static void *filewrite(void *args)
// int filewrite(int acceptfd, MSG *msg, char const filename[])
{
  FILE *fp;
  char *buffer;
  size_t result;
  thread_arg *arg;
  int acceptfd;
  MSG *msg;
  char filename[128];
  arg = (thread_arg *)args;
  acceptfd = arg->acceptfd;
  strcpy(filename, arg->file_name);
  msg = arg->msg;

  fprintf(stdout, "接收文件大小为：\n");
  if (recv(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("Do not know file size");
    return NULL;
  }

  long lSize;
  lSize = atol(msg->data);
  fprintf(stdout, "%ld\n", lSize);

  buffer = (char *)malloc(sizeof(char) * lSize);

  if (buffer == NULL)
  {
    perror("failed malloc");
    exit(1);
  }

  if ((fp = fopen(filename, "wb")) == NULL)
  {
    perror("failed open file");
    exit(1);
  }

  char *buffer_pointer = buffer;
  size_t length = lSize;
  for (;;)
  {

    size_t result = recv(acceptfd, buffer_pointer, length, MSG_WAITALL);
    if (length < 0 || result <= 0)
    {
      break;
    }

    length -= result;
    buffer_pointer += result;
  }

  result = fwrite(buffer, sizeof(char), lSize, fp);
  if (result != lSize)
  {
    perror("failed fwrite");
    exit(1);
  }

  fprintf(stdout, "下载完毕\n");
  free(buffer);
  fclose(fp);

  return NULL;
}

//客户端显示服务器目录下的文件
int client_listfile(int acceptfd, MSG *msg)
{
  msg->type = L;

  if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed to send");
    return 0;
  }
  while (1)
  {
    if (recv(acceptfd, msg, sizeof(MSG), 0) < 0)
    {
      perror("failed to recv");
      return 0;
    }
    if ('\0' == msg->data[0])
    {
      break;
    }
    fprintf(stdout, "%s\n", msg->data);
  }
  fprintf(stdout, "服务器目录已经接收完毕\n");
  return 0;
}

//客户端列出当前目录下的文件
int fileshow()
{
  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir("./")) == NULL)
  {
    printf("failed opendir");
    return -1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    printf("%s  ", entry->d_name);
  }
  puts(" ");
  return 0;
}

//显示帮助
void help()
{
  printf("***************************************************\n");
  printf("******** 输入/功能 *********************************\n");
  printf("********list :查看服务器所在目录的所有文件  *********\n");
  printf("********get filename下载服务器目录的文件   **********\n");
  printf("********put filename: 上传文件到服务器***************\n");
  printf("********quit :关闭客户端 ****************************\n");
  printf("*****************************************************\n");
  printf("\n");
}
