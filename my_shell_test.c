#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#define HISTORY_MAX   100
#define PATHNAME_MAX  200
#define INPUT_MAX     200
#define ARG_MAX       10
#define ARGC_MAX      64

int hiscount=0;

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
    char *input=(char *)malloc(sizeof(char)*INPUT_MAX);
    while(1)
    {
        getcwd(cwdpath,PATHNAME_MAX);
        strcpy(hispath[hiscount++],cwdpath);
        printf("%s\n",hispath[0]);
        printf("\033[1;34m%s\033[0m\n",cwdpath);
        input = readline("$ ");
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