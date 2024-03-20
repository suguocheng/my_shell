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
void exe(char *args[],int p);
void exe_com(char *args[],int argcount);
void change_dir(char *args[],char *hispath[]);
int main()
{
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
        input=readline("$ ");
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
        exe_com(args,argcount);
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
    if(args[1]==NULL||strcmp(args[1],"~")==0)
    {
        chdir("/home/pluto");
    }
    else if(strcmp(args[1],"-")==0)
    {
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
void exe_com(char *args[],int argcount)
{
    pid_t pid=fork();
    if(pid==0)
    {
        int fd;
        for(int i=0;i<argcount;i++)
        {
            if(strcmp(args[i],"&")==0)
            {
                
            }
        }
        for(int i=0;i<argcount;i++)
        {
            if(strcmp(args[i],"<")==0)
            {
                fd=open(args[i+1],O_RDONLY);
                dup2(fd,STDIN_FILENO);
                close(fd);
            }
            if(strcmp(args[i],">")==0)
            {
                fd=open(args[i+1],O_WRONLY|O_CREAT|O_TRUNC,0644);
                dup2(fd,STDOUT_FILENO);
                close(fd);
            }
            if(strcmp(args[i],">>")==0)
            {
                fd=open(args[i+1],O_WRONLY|O_CREAT|O_APPEND,0644);
                dup2(fd,STDOUT_FILENO);
                close(fd);
            }
            if(strcmp(args[i],"|")==0)
            {
                int fd[2];
                if(pipe(fd)==-1)
                {
                    printf("pipe error");
                }
                pid_t pid2=fork();
                if(pid2==0)
                {
                    close(fd[0]);
                    dup2(fd[1],STDOUT_FILENO);
                }
                else if(pid2>0)
                {
                    waitpid(pid2,NULL,0);
                    close(fd[1]);
                    dup2(fd[0],STDIN_FILENO); 
                }
                else
                {
                    printf("fork error");
                }
            }
        }
    }
    else if(pid>0)
    {
        waitpid(pid,NULL,0);
    }
    else
    {
        printf("fork error");
    }
}
void exe(char *commands[],int p)
{
    pid_t pid=fork();
    if(pid==0)
    {
        execvp(commands[0],commands);
    }
    else if(pid>0)
    {
        waitpid(pid,NULL,0);
    }
    else
    {
        printf("fork error");
    }
}