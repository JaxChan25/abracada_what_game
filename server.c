#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "game.h"

#define BUF_SIZE 100

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int PLAYER_NUM;
pthread_mutex_t mutx;
Game g_ins;
Game *gptr = &g_ins;

int main(int argc, char *argv[])
{
    printf("start server!\n"); //用\n刷新缓冲区
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz = sizeof(clnt_adr);
    pthread_t t_id;
    if (argc != 3)
    {
        printf("Usage : %s <port> <player_num>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL); //创建互斥锁
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    PLAYER_NUM = atoi(argv[2]);
    gptr->player_num = PLAYER_NUM;
    init_player_list(gptr);
    gptr->remain_cards = NULL;

    // 进入游戏状态
    while (1)
    {
        switch (gptr->game_state)
        {
        case CONNECT_PLAYERS:
            for (int i = 0; i < PLAYER_NUM; i++)
            {
                printf("准备初始化id=%d的角色\n", i);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz); // 这个是阻塞的
                // 初始化角色
                gptr->player_list[i].id = i;
                gptr->player_list[i].player_sock = clnt_sock; //写入新连接
                char msg[BUF_SIZE];
                int str_len = read(clnt_sock, msg, sizeof(msg));
                if (str_len == 0)
                {
                    error_handling("player初始化失败");
                }
                msg[str_len] = 0;
                gptr->player_list[i].name = (char *)malloc(str_len);
                strcpy(gptr->player_list[i].name, msg);
                sprintf(msg, "[SERVER]初始化角色,id = %d, name=%s\n", gptr->player_list[i].id, gptr->player_list[i].name);
                send_msg(msg, sizeof(msg));

                pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);       //创建线程为新客户端服务，并且把clnt_sock作为参数传递
                pthread_detach(t_id);                                               //引导线程销毁，不阻塞
                printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr)); //客户端连接的ip地址
            }
            gptr->game_state = INIT_GAME;
            break;
        case INIT_GAME:
            printf("进入游戏初始化");
            for (size_t i = 0; i < PLAYER_NUM; i++)
            {
                gptr->player_list[i].hp = 6;
            }
            // 初始化牌堆
            if (gptr->remain_cards != NULL)
            {
                free(gptr->remain_cards);
            }
            gptr->remain_cards = make_remain_cards();
            // 发牌
            init_players_cards(gptr);
            gptr->game_state = RUN_GAME;
            break;
        case RUN_GAME:
            /* code */
            break;
        case CALCULATE_POINTS:
            /* code */
            break;

        default:
            break;
        }
    }

    printf("finish game!\n");
    free_player_list(gptr);
    free(gptr);
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    //会卡在这里一致循环
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    return NULL;
}
void send_msg(char *msg, int len) //向连接的所有客户端发送消息
{
    int i;
    for (i = 0; i < PLAYER_NUM; i++)
    {
        int player_sock = gptr->player_list[i].player_sock;
        if (player_sock != -1)
        {
            printf("给id= %d,发送消息,%s", i, msg);
            write(player_sock, msg, len);
        }
    }
}
void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}