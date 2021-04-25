#include "game.h"

void init_player_list(Game *gptr)
{
    printf("init_player_list,%d\n", gptr->player_num);
    gptr->player_list = (Player *)malloc(sizeof(Player) * gptr->player_num);
    for (int i = 0; i < gptr->player_num; i++)
    {
        gptr->player_list[i].player_sock = -1;
    }
}

void free_player_list(Game *gptr)
{
    printf("free_player_list\n");
    // 释放名字空间
    for (int i = 0; i < gptr->player_num; i++)
        free(gptr->player_list[0].name);
    // 释放玩家列表
    free(gptr->player_list);
}

void print_player(Player *p)
{
    printf("id:%d,sock:%d,hp:%d,name:%s\n", p->id, p->player_sock, p->hp, p->name);
}

int *make_remain_total_cards()
{
    int *p;
    p = (int *)malloc(CAR_TYPE * sizeof(int));
    for (int i = 0; i < CAR_TYPE; i++)
    {
        p[i] = i + 1;
    }
    return p;
}

