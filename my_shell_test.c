#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>


#define PATHNAME_MAX  200
#define COMMAND_MAX   200
#define ARG_MAX       10

void change_dir(char *argv[]);
int main()
{
    while(1)
    {
        char *cwdpath=(char *)malloc(sizeof(char)*PATHNAME_MAX);
        getcwd(cwdpath,PATHNAME_MAX);
        printf("%s\n",cwdpath);
        char *input=readline("$ ");
        if((input)!=NULL) 
        {
            add_history(input);
        }
        char *tem_command=strdup(input);
        int i=0;
        int argc=0;
        if(input==NULL)
        {
            return 0;
        }
        //求出argc
        char *temp=strtok(tem_command," ");
        while(temp!=NULL)
        {
            argc++;
            temp=strtok(tem_command," ");
        }
        //为参数分配空间
        char **argv=(char **)malloc(sizeof(char *)*argc);
        for(int i=0;i<argc;i++)
        {
            argv[i]=(char *)malloc(sizeof(char)*ARG_MAX);
        }
        //存入参数
        char *temp2=strtok(input," ");
        while(temp2!=NULL)
        {
            argv[i++]=temp2;
            temp2=strtok(input," ");
        }
        if(strcmp(argv[0],"cd")==0)
        {
            change_dir(argv);
        }
        if(strcmp(argv[0],"exit")==0)
        {
            break;
        }
    }
    return 0;
}
void change_dir(char *argv[])
{

}