#if !defined(_COMMON_H_)
#define _COMMON_H_

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <sstream>
#include <string>

#define I_WANNA_JOIN_SEMAPHORE "Join_wanna"
#define JOIN_SHARED_MEM_NAME "Join"
#define GET_YOU_DATA_SEMAPHORE "Join_get"

struct join_t {
    sem_t cs;
    char name[100];
    pid_t server_pid;
    pid_t player_pid;
    bool connection_refused;
};
struct player_t{
    sem_t cs;
    int direction;
    char map[25][51][4];
    int map_size_y;
    int map_size_x;
    int coins_carried;
    int coins_brought;
    int death_count;
    int pos_y;
    int pos_x;
    int round_number;
};

#endif // _COMMON_H_