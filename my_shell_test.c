#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATHNAME_MAX 200
#define COMMAND_MAX  200
#define ARG_MAX      10     

void read_command();
int main(int argc,char*argv[])
{
    while(1)
    {
        char *cwdpath=(char *)malloc(sizeof(char)*PATHNAME_MAX);
        getcwd(cwdpath,PATHNAME_MAX);
        printf("%s\n",cwdpath);
        read_command();

    }
    return 0;
}
void read_command()
{
    char *command=(char *)malloc(sizeof(char)*COMMAND_MAX);
    fgets(command,200,stdin);
    char *tem_command;
    int i=0;
    int argc=0;
    //求出argc
    char *temp=strtok(tem_command,' ');
    while(temp!=NULL)
    {
        argc++;
        temp=strtok(tem_command,' ');
    }
    //为参数分配空间
    char **argv=(char **)malloc(sizeof(char *)*argc);
    for(int i=0;i<argc;i++)
    {
        argv[i]=(char *)malloc(sizeof(char)*ARG_MAX);
    }
    //存入参数
    char *temp=strtok(command,' ');
    while(temp!=NULL)
    {
        argv[i++]=temp;
        temp=strtok(command,' ');
    }
    //判断参数
    int count=0;
    for(int i=0;i<argc;i++)
    {
        if(argv[i]==">"||argv[i]=="<"||argv[i]==">>"||argv[i]=="<<"||argv[i]=="|"||argv[i]=="|")
        {
            count=1;
        }
    }
    if(count==0)
    {

    }
    else
    {

    }
}