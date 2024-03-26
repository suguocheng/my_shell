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
#include <signal.h>

#define HISTORY_MAX   100
#define PATHNAME_MAX  200
#define INPUT_MAX     200
#define ARG_MAX       10
#define ARGC_MAX      64

int hiscount=0;

void exe_com(char *args[],int argcount);
void change_dir(char *args[],char *hispath[],int argcount);
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

    signal(SIGINT, SIG_IGN);

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
            change_dir(args,hispath,argcount);
            continue;
        }
        if(strcmp(args[0],"exit")==0)
        {
            break;
        }
        //debug
        // for(int i=0;i<argcount;i++)
        // {
        //     printf("args[%d]=%s\n",i,args[i]);
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
            printf("fork1 error");
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
void change_dir(char *args[],char *hispath[],int argcount)
{
    if(strcmp(args[1],"~")==0||argcount==1)
    {
        chdir("/home/pluto");
    }
    else if(strcmp(args[1],"-")==0)
    {
        printf("%s\n\n",hispath[hiscount-2]);
        chdir(hispath[hiscount-2]);
    }
    else if(argcount>1&&chdir(args[1])==-1)
    {
        printf("cd error\n");
    }
}
void exe_com(char *args[], int argcount)
{
    int is_background=0;
    int pipepos=-1;
    
    //debug
    // printf("pipepos=%d\n",pipepos);

    int i=0;
    int pfd[2];
    for(int a=0;a<argcount;a++)
    {
        if (args[a]!=NULL&&strcmp(args[a],"|")==0)
        {
            if(pipe(pfd)==-1)
            {
                printf("pipe error");
            }
            pipepos=a;
            break;
        }
    }
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        int fd;
        for(int a=0;a<argcount;a++)
        {
            if(args[i]!=NULL&&strcmp(args[a],"&")==0)
            {
                is_background=1;
                args[a]=NULL;
                break;
            }
        }
        for(i=0;i<argcount;i++)
        {
            if(args[i]!=NULL&&strcmp(args[i],"<")==0)
            {
                fd=open(args[i+1],O_RDONLY);
                dup2(fd,STDIN_FILENO);
                close(fd);
                args[i]=NULL;
                args[i+1]=NULL;
            }
            else if(args[i]!=NULL&&strcmp(args[i],">")==0)
            {
                fd=open(args[i+1],O_WRONLY|O_CREAT|O_TRUNC,0644);
                dup2(fd,STDOUT_FILENO);
                close(fd);
                args[i]=NULL;
                args[i+1]=NULL;
            }
            else if(args[i]!=NULL&&strcmp(args[i],">>")==0)
            {
                fd=open(args[i+1],O_WRONLY|O_CREAT|O_APPEND,0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                args[i]=NULL;
                args[i+1]=NULL;
            }
            else if(args[i]!=NULL&&strcmp(args[i],"|")==0)
            {
                close(pfd[0]);
                dup2(pfd[1],STDOUT_FILENO);
                close(pfd[1]);
                break;
            }
        }

        //debug
        // printf("pipepos=%d\n",pipepos);
        // printf("i=%d\n",i);
        //debug
        // for(int b=0;b<argcount;b++)
        // {
        //     if(args[b]!=NULL)
        //     printf("arg[%d]=%s\n",b,args[b]);
        // }
        
        pid_t pid2;
        pid2=fork();
        if(pid2==0)
        {
            char *commands[ARGC_MAX];

            //malloc
            // for(int a=0;a<ARGC_MAX;a++)
            // {
            //     commands[a]=(char*)malloc(sizeof(char)*ARG_MAX);
            // }

            int k=0;
            for(int j=0;j<i;j++)
            {
                if(args[j]!=NULL)
                {
                    // strcpy(commands[k],args[j]);
                    commands[k]=args[j];
                    k++;
                }
            }

            //debug
            // printf("k=%d\n",k);

            commands[k]=NULL;

            //debug
            // for(int c=0;c<k;c++)
            // {
            //     if(commands[c]!=NULL)
            //     printf("commands[%d]=%s\n",c,commands[c]);
            // }

            execvp(commands[0],commands);
            exit(0);
        }
        else if(pid2>0)
        {
            if(is_background==0)
            {
                waitpid(pid2,NULL,0);
            }
        }
        else
        {
            printf("fork3 error\n");
            perror("fork");
        }
    }
    else if(pid>0)
    {    
        waitpid(pid,NULL,0);
        close(pfd[1]);
        dup2(pfd[0],STDIN_FILENO);
        if(pipepos>0)
        {
            //debug
            // printf("递归pipepos=%d\n",pipepos);
            exe_com(args+pipepos+1,argcount-pipepos-1);
        }
    }
    else
    {
        printf("fork2 error");
    }
}
