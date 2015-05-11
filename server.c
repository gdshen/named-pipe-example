#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fifo.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>

char pipe_server_name[100];
char pipe_client_writer_name[100];
char pipe_client_reader_name[100];
char buff[4096];

void from_client_to_server(int client_pid) {
    CREATE_CLIENT_WRITER_NAME(pipe_client_writer_name, client_pid)

    int pipe_client_writer_fd = open(pipe_client_writer_name, O_RDONLY);
    read(pipe_client_writer_fd, buff, sizeof(buff));
    close(pipe_client_writer_fd);
}

void from_server_to_client(int client_pid) {
    CREATE_CLIENT_READER_NAME(pipe_client_reader_name, client_pid)

    int pipe_client_reader_fd = open(pipe_client_reader_name, O_WRONLY);
    write(pipe_client_reader_fd, buff, sizeof(buff));
    close(pipe_client_reader_fd);
}


void getdir(char *user_input, char *dir) {
    user_input += 3;
    while (*user_input != '\n') {
        *dir++ = *user_input++;
    }
    *dir = 0;
}

void sigint_handler(int sig) {
    printf("开始清除server创建的fifo...\n");
    CREATE_SERVER_FIFO_NAME(pipe_server_name);
    if (unlink(pipe_server_name) == 0) {
        printf("清除成功\n");
    }
    exit(0);
}

int main() {

    //定义pipe_server_name
    CREATE_SERVER_FIFO_NAME(pipe_server_name)

    //创建对应的命名管道，权限设置为666
    int state = mkfifo(pipe_server_name, FIFO_PERMISSIONS);

    //注册新号处理函数到内核
    signal(SIGINT, sigint_handler);

    // 打开 pipe_server_fd并读取第一个client的pid
    while (1) {
        int pipe_server_fd = open(pipe_server_name, O_RDONLY);

        int client_pid;
        read(pipe_server_fd, &client_pid, sizeof(int));
        close(pipe_server_fd);

        int pid_t = fork();

        if (pid_t == 0) {
            printf("从server fork得到子进程pid %d是处理来自client %d的连接\n", getpid(), client_pid);

            while (1) {
                // server fork后的进程用来处理来自client的连接
                from_client_to_server(client_pid);
                if (!strcmp(buff, "exit")) {
                    printf("执行exit\n");
                    from_server_to_client(client_pid);
                    exit(0);
                } else if (!strcmp(buff, "pwd")) {
                    printf("执行pwd\n");
                    FILE *fp;
                    fp = popen("pwd", "r");
                    fgets(buff, sizeof(buff), fp);
                    pclose(fp);
                    from_server_to_client(client_pid);
                } else if (!strncmp(buff, "cd", 2)) {
                    printf("执行%s",buff);
                    char dir[100];
                    getdir(buff, dir);
                    chdir(dir);
                } else if (!strcmp(buff, "dir")) {
                    printf("执行dir\n");
                    FILE *fp;
                    fp = popen("ls -lF", "r");

                    char c;
                    int i = 0;
                    c = fgetc(fp);
                    while(c != EOF){
                        buff[i++] = c;
                        c = fgetc(fp);
                    }
                    buff[i] = 0;

                    pclose(fp);
                    from_server_to_client(client_pid);
                }
                else {
                    printf("输入命令有误,请重新参考help重新输入\n");
                }
            }
        }
    }
}