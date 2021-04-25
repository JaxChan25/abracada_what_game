#ifndef _GAME_H
#define _GAME_H

#include <stdio.h>
#include <stdlib.h>

#define CAR_TYPE 8

enum GAME_STATE
{
    CONNECT_PLAYERS,
    INIT_GAME,
    RUN_GAME,
    CALCULATE_POINTS
};

struct SPlayer
{
    int id;
    char *name;
    int player_sock;
    int hp;
};
typedef struct SPlayer Player;

struct SGame
{
    int player_num;
    enum GAME_STATE game_state;
    int handcard[5];
    Player *player_list;
};
typedef struct SGame Game;

void init_player_list(Game *gptr);
void free_player_list(Game *gptr);
void print_player(Player *p);

#endif