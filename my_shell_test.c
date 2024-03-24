#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <sys/wait.h>

#define HISTORY_MAX   100
#define PATHNAME_MAX  200
#define INPUT_MAX     200
#define ARG_MAX       10
#define ARGC_MAX      64

int hiscount=0;

//重定向输入函数
//重定向输出覆盖函数
//重定向输出追加函数
//管道函数
//后台函数
//搜索命令函数
// void exe(char *args[]);
void exe_com(char *args[],int argcount);
void change_dir(char *args[],char *hispath[]);
int main()
{
    puts(
        "\033[32m" "\033[1m" "\n"
        " _       __     __                            __                          _____ __         ____\n"
        "| |     / /__  / /________  ____ ___  ___    / /_____    ____ ___  __ __ / ___// /_  ___  / / /\n"
        "| | /| / / _ \\/ / ___/ __ \\/ __ `__ \\/ _ \\  / __/ __ \\  / __ `__ \\/ / / /\\__ \\/ __ \\/ _ \\/ / / \n"
        "| |/ |/ /  __/ / /__/ /_/ / / / / / /  __/ / /_/ /_/ / / / / / / / /_/ /___/ / / / /  __/ / / \n"
        "|__/|__/\\___/_/\\___/\\____/_/ /_/ /_/\\___/  \\__/\\____/ /_/ /_/ /_/\\__, //____/_/ /_/\\___/_/_/ \n"
        "                                                                /____/ \n"
        "\033[0m"
    );
    char *cwdpath=(char *)malloc(sizeof(char)*PATHNAME_MAX);
    char **hispath=(char **)malloc(sizeof(char*)*HISTORY_MAX);
    for(int i=0;i<HISTORY_MAX;i++)
    {
        hispath[i]=(char *)malloc(sizeof(char)*PATHNAME_MAX);
    }
    char **args=(char **)malloc(sizeof(char *)*ARGC_MAX);
    for(int i=0;i<ARGC_MAX;i++)
    {
        args[i]=(char *)malloc(sizeof(char)*ARG_MAX);
    }
    char *input=NULL;
    while(1)
    {
        getcwd(cwdpath,PATHNAME_MAX);
        strcpy(hispath[hiscount++],cwdpath);
        printf("\033[1;34m%s\033[0m\n",cwdpath);
        if(input!= NULL)
        {
            free(input);
            input=NULL;
        }
        input=readline("");
        if(input!=NULL) 
        {
            add_history(input);
        }
        char *temp=strtok(input," ");
        int argcount=0;
        while(temp!=NULL)
        {
            args[argcount++]=temp;
            temp=strtok(NULL," ");
        }
        if(strcmp(args[0],"cd")==0)
        {
            change_dir(args,hispath);
            continue;
        }
        if(strcmp(args[0],"exit")==0)
        {
            break;
        }
        //debug
        // for(int i=0;i<argcount;i++)
        // {
        //     printf("%s\n",args[i]);
        // }

        pid_t pid;
        pid = fork();
        if (pid == 0)
        {
            exe_com(args,argcount);
            exit(0);
        }
        else if (pid > 0)
        {
            waitpid(pid, NULL, 0);
        }
        else
        {
            printf("fork error");
        }
    }
    free(cwdpath);
    for(int i=0;i<HISTORY_MAX;i++)
    {
        free(hispath[i]);
    }
    free(hispath);
    for(int i=0;i<ARGC_MAX;i++)
    {
        free(args[i]);
    }
    free(args);
    free(input);
    return 0;
}
void change_dir(char *args[],char *hispath[])
{
    if(strcmp(args[1],"~")==0)
    {
        chdir("/home/pluto");
    }
    else if(strcmp(args[1],"-")==0)
    {
        printf("%s\n\n",hispath[hiscount-2]);
        chdir(hispath[hiscount-2]);
    }
    else if(chdir(args[1])==-1)
    {
        printf("cd error\n");
    }
    else
    {
        chdir(args[1]);
    }
}
void exe_com(char *args[], int argcount)
{
    int is_background = 0;
    int fd;
    int pfd[2];
    int i;
    int pipepos=-1;
    for (int i = 0; i < argcount; i++)
    {
        if (strcmp(args[i], "&") == 0)
        {
            is_background = 1;
            args[i] = NULL;
            break;
        }
    }
    for (i = 0; i < argcount; i++)
    {
        if (strcmp(args[i], "<") == 0)
        {
            fd = open(args[i + 1], O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
            // args[i+1] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            // args[i+1] = NULL;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            // args[i+1] = NULL;
        }
        else if (strcmp(args[i], "|") == 0)
        {
            if (pipe(pfd) == -1)
            {
                printf("pipe error");
            }
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            close(pfd[1]);
            pipepos=i;
            break;
        }
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        if(pipepos>0)
        {
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
        }
        char *commands[ARGC_MAX];
        int k=0;
        for (int j = 0; j < i; j++)
        {
            commands[k] = args[j];
            k++;
            // if(commands[k]!=NULL)
            // printf("%s\n",commands[k]);//debug
        }
        commands[k] = NULL;
        execvp(commands[0], commands);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        if (is_background == 0)
        {
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        printf("fork error");
    }
    //递归
    if(pipepos>0)
    {
        exe_com(args+i+1, argcount-i-1);
    }
}
