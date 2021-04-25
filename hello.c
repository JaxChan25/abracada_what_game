#include <stdio.h>
#define NAME_SIZE 20
#define BUF_SIZE 100
char msg[BUF_SIZE];
char name[NAME_SIZE] = "[DEFAULT]";

int main()
{
    printf("hello,world!\n");

    fgets(msg, BUF_SIZE, stdin);
    char name_msg[NAME_SIZE + BUF_SIZE];
    sprintf(name_msg, "%s %s", name, msg);
    printf("%d\n", sizeof(name_msg));
}