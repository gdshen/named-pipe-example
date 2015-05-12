#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fifo.h"
#include <unistd.h>
#include <string.h>

char pipe_server_name[100];
char pipe_client_writer_name[100];
char pipe_client_reader_name[100];
char buff[4096];

void help() {
    printf("help 打开帮助\n");
    printf("lpwd 显示client端的工作路径\n");
    printf("lcd  切换client进程的目录,多次cd会有问题\n");
    printf("ldir 显示client端的目录内容\n");
    printf("pwd  显示client_server的工作路径\n");
    printf("cd   切换client_server进程的目录,多次cd会有问题\n");
    printf("dir  显示client_server的目录内容\n");
    printf("exit 结束这个client\n");
}

void from_client_to_server() {
    CREATE_CLIENT_WRITER_NAME(pipe_client_writer_name, getpid())

    int pipe_client_writer_fd = open(pipe_client_writer_name, O_WRONLY);
    write(pipe_client_writer_fd, buff, sizeof(buff));
    close(pipe_client_writer_fd);
}

void from_server_to_client() {
    CREATE_CLIENT_READER_NAME(pipe_client_reader_name, getpid())

    int pipe_client_reader_fd = open(pipe_client_reader_name, O_RDONLY);
    read(pipe_client_reader_fd, buff, sizeof(buff));
   // printf("%s\n", buff);
    close(pipe_client_reader_fd);
}

void exit_program() {
    strcpy(buff,"exit");
    from_client_to_server();
    from_server_to_client();

    unlink(pipe_client_writer_name);
    unlink(pipe_client_reader_name);
    exit(0);
}

void pwd() {
    strcpy(buff,"pwd");
    from_client_to_server();
    from_server_to_client();
    printf("%s", buff);
}

void getdir(char *user_input, char *dir) {
    user_input += 4;
    while (*user_input != '\n') {
        *dir++ = *user_input++;
    }
    *dir = 0;
}

void user_input_handler(char *user_input) {
    if (!strcmp(user_input, "help\n")) {
        help();
    } else if (!strcmp(user_input, "lpwd\n")) {
        system("pwd");
    } else if (!strcmp(user_input, "ldir\n")) {
        system("ls -lF");
    } else if (!strcmp(user_input, "pwd\n")) {
        pwd();
    } else if (!strcmp(user_input, "exit\n")) {
        exit_program();
    } else if (!strncmp(user_input, "lcd", 3)) {
        char dir[100];
        getdir(user_input, dir);
        chdir(dir);
    } else if (!strncmp(user_input, "cd", 2)) {
        // printf("%s\n",user_input);
        strcpy(buff,user_input);
        from_client_to_server();
    } else if (!strcmp(user_input, "dir\n")) {
        strcpy(buff,"dir");
        from_client_to_server();
        from_server_to_client();
        printf("%s", buff);
    }
}


int main() {
    //定义pipe_server_name, pipe_client_writer_name, pipe_client_reader_name, 并 穿创建相对应的pipe
    CREATE_SERVER_FIFO_NAME(pipe_server_name)
    CREATE_CLIENT_WRITER_NAME(pipe_client_writer_name, getpid())
    CREATE_CLIENT_READER_NAME(pipe_client_reader_name, getpid())

    mkfifo(pipe_client_writer_name, FIFO_PERMISSIONS);
    mkfifo(pipe_client_reader_name, FIFO_PERMISSIONS);

    //在pipe_server_name中写入client的pid
    int pipe_server_fd = open(pipe_server_name, O_WRONLY);
    int client_pid = getpid();
    write(pipe_server_fd, &client_pid, sizeof(int));
    close(pipe_server_fd);

    printf(CLIENT_PROMPT);
    char user_input[100];
    while (1) {
        fgets(user_input, sizeof(user_input), stdin);
        user_input_handler(user_input);
        printf(CLIENT_PROMPT);
    }
    return 0;
}