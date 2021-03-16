#include "common.h"
#include "server.h"


std::vector<Player*> players;
std::vector<struct treasure_t> treasures;
std::vector<sem_t*> beasts_post_semaphores;
std::vector<sem_t*> beasts_get_semaphores;
int beast_count = 0;
int round_number = 0;
pthread_mutex_t map_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t beasts_mutex = PTHREAD_MUTEX_INITIALIZER;
pid_t server_pid = getpid();


bool game_is_on = true;
char map_for_treasures[25][51][4] = {
        {"█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█"},
        {"█","c"," "," ","█"," "," "," "," "," "," "," ","█"," "," "," ","#","#","#","#","#"," "," "," "," "," "," "," ","█"," "," "," "," "," "," "," "," "," ","█","c","c","c","c","c","c","c","█"," "," "," ","█"},
        {"█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█"," ","█","█","█"," ","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█"," ","t"," ","█"},
        {"█"," ","█"," "," "," ","█"," ","█"," ","█"," "," "," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"},
        {"█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","#","#","#","█","█","█","█","█"," ","█"," ","█"," ","█","█","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█","█","█"," ","█","█","█"," ","█"},
        {"█"," ","█","T","█"," "," "," ","█"," "," "," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," ","#","#"," "," ","█"," "," "," "," "," "," ","c","█"," "," "," "," "," "," "," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," "," "," "," "," "," "," "," "," ","█"," ","█"," "," "," "," "," "," "," ","█"," ","█"," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," ","█"," "," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█"," ","█"},
        {"█"," ","█","T","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," "," "," ","█"," "," "," "," "," "," "," ","█"," ","█"," "," ","c","█"," "," ","A"," "," ","█"," ","█"," "," "," ","█"," ","█"," "," "," "," "," "," "," ","█"," ","█"," "," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█","█","#","█","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","█","█"," ","█"," ","█"," ","█","█","█"," ","█"},
        {"█"," ","█"," "," "," "," ","#","█"," "," "," ","█"," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," "," "," "," "," ","█"," ","█"," ","█"," "," "," ","█"},
        {"█"," ","█"," ","█"," ","#","#","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█"},
        {"█"," ","█"," ","█","#","#"," ","█"," "," "," "," ","#"," "," ","█"," "," "," ","█"," ","█"," "," ","#","#","#"," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","#"," "," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█"," ","█","█","#","█","█","█","█"," ","█","█","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","#"," "," "," "," "," "," ","█"," "," "," ","█"," ","█"," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," ","█"," "," "," "," "," "," "," ","#","#","█"," "," "," ","█"},
        {"█"," ","█","█","█","█","█","█","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█","█","█","█","█"," ","█"," ","#","#","█","█","█"," ","█"},
        {"█"," ","█","#"," "," "," "," "," "," ","█"," ","█"," "," "," "," "," ","█"," "," "," "," "," ","█"," "," "," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," ","#","#"," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█"," ","█"," ","█","█","█","█","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","#","█","█","█","█","█"," ","█"},
        {"█","#","#","#","█"," "," "," "," "," ","█"," "," "," "," "," "," "," "," "," ","█"," "," ","#","#","#","█","#","#"," ","█"," "," "," "," "," ","█"," ","█"," "," "," ","█","#","#","#","#","#","#"," ","█"},
        {"█","c","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","#","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","#"," "," "," "," ","#"," ","█"},
        {"█","c","c","c","█"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ","#","#","#","#","#","#","█","#","#"," "," "," "," "," "," "," "," "," ","█"," "," "," "," ","#","#"," "," "," ","█"},
        {"█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█"}
    };
char map_blueprint[25][51][4] = {
        {"█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█"},
        {"█","c"," "," ","█"," "," "," "," "," "," "," ","█"," "," "," ","#","#","#","#","#"," "," "," "," "," "," "," ","█"," "," "," "," "," "," "," "," "," ","█","c","c","c","c","c","c","c","█"," "," "," ","█"},
        {"█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█"," ","█","█","█"," ","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█"," ","t"," ","█"},
        {"█"," ","█"," "," "," ","█"," ","█"," ","█"," "," "," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"},
        {"█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","#","#","#","█","█","█","█","█"," ","█"," ","█"," ","█","█","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█","█","█"," ","█","█","█"," ","█"},
        {"█"," ","█","T","█"," "," "," ","█"," "," "," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," ","#","#"," "," ","█"," "," "," "," "," "," ","c","█"," "," "," "," "," "," "," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," "," "," "," "," "," "," "," "," ","█"," ","█"," "," "," "," "," "," "," ","█"," ","█"," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," ","█"," "," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█"," ","█"},
        {"█"," ","█","T","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," "," "," ","█"," "," "," "," "," "," "," ","█"," ","█"," "," ","c","█"," "," ","A"," "," ","█"," ","█"," "," "," ","█"," ","█"," "," "," "," "," "," "," ","█"," ","█"," "," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█","█","#","█","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","█","█"," ","█"," ","█"," ","█","█","█"," ","█"},
        {"█"," ","█"," "," "," "," ","#","█"," "," "," ","█"," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," "," "," "," "," ","█"," ","█"," ","█"," "," "," ","█"},
        {"█"," ","█"," ","█"," ","#","#","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█"},
        {"█"," ","█"," ","█","#","#"," ","█"," "," "," "," ","#"," "," ","█"," "," "," ","█"," ","█"," "," ","#","#","#"," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","#"," "," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█"," ","█","█","#","█","█","█","█"," ","█","█","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","#"," "," "," "," "," "," ","█"," "," "," ","█"," ","█"," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," ","█"," "," "," "," "," "," "," ","#","#","█"," "," "," ","█"},
        {"█"," ","█","█","█","█","█","█","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█","█","█","█","█"," ","█"," ","#","#","█","█","█"," ","█"},
        {"█"," ","█","#"," "," "," "," "," "," ","█"," ","█"," "," "," "," "," ","█"," "," "," "," "," ","█"," "," "," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," ","#","#"," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█"," ","█"," ","█","█","█","█","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","#","█","█","█","█","█"," ","█"},
        {"█","#","#","#","█"," "," "," "," "," ","█"," "," "," "," "," "," "," "," "," ","█"," "," ","#","#","#","█","#","#"," ","█"," "," "," "," "," ","█"," ","█"," "," "," ","█","#","#","#","#","#","#"," ","█"},
        {"█","c","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","#","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","#"," "," "," "," ","#"," ","█"},
        {"█","c","c","c","█"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ","#","#","#","#","#","#","█","#","#"," "," "," "," "," "," "," "," "," ","█"," "," "," "," ","#","#"," "," "," ","█"},
        {"█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█"}
    };
char map[25][51][4] = {
        {"█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█"},
        {"█","c"," "," ","█"," "," "," "," "," "," "," ","█"," "," "," ","#","#","#","#","#"," "," "," "," "," "," "," ","█"," "," "," "," "," "," "," "," "," ","█","c","c","c","c","c","c","c","█"," "," "," ","█"},
        {"█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█"," ","█","█","█"," ","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█"," ","t"," ","█"},
        {"█"," ","█"," "," "," ","█"," ","█"," ","█"," "," "," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"},
        {"█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","#","#","#","█","█","█","█","█"," ","█"," ","█"," ","█","█","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█","█","█"," ","█","█","█"," ","█"},
        {"█"," ","█","T","█"," "," "," ","█"," "," "," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," ","#","#"," "," ","█"," "," "," "," "," "," ","c","█"," "," "," "," "," "," "," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," "," "," "," "," "," "," "," "," ","█"," ","█"," "," "," "," "," "," "," ","█"," ","█"," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," ","█"," "," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█"," ","█"},
        {"█"," ","█","T","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," "," "," "," "," ","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," "," "," ","█"," "," "," "," "," "," "," ","█"," ","█"," "," ","c","█"," "," ","A"," "," ","█"," ","█"," "," "," ","█"," ","█"," "," "," "," "," "," "," ","█"," ","█"," "," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█","█","#","█","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","█","█"," ","█"," ","█"," ","█","█","█"," ","█"},
        {"█"," ","█"," "," "," "," ","#","█"," "," "," ","█"," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," "," "," "," "," ","█"," ","█"," ","█"," "," "," ","█"},
        {"█"," ","█"," ","█"," ","#","#","█"," ","█","█","█"," ","█","█","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█"," ","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█"},
        {"█"," ","█"," ","█","#","#"," ","█"," "," "," "," ","#"," "," ","█"," "," "," ","█"," ","█"," "," ","#","#","#"," "," ","█"," "," "," ","█"," "," "," ","█"," "," "," "," "," ","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","#"," "," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"," ","█"," ","█","█","#","█","█","█","█"," ","█","█","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█"," ","█"},
        {"█"," ","█"," ","█","#"," "," "," "," "," "," ","█"," "," "," ","█"," ","█"," ","█"," "," "," ","█"," "," "," "," "," ","█"," "," "," ","█"," ","█"," "," "," "," "," "," "," ","#","#","█"," "," "," ","█"},
        {"█"," ","█","█","█","█","█","█","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█","█","█"," ","█","█","█","█","█","█","█"," ","█"," ","█","█","█","█","█"," ","█"," ","#","#","█","█","█"," ","█"},
        {"█"," ","█","#"," "," "," "," "," "," ","█"," ","█"," "," "," "," "," ","█"," "," "," "," "," ","█"," "," "," "," "," "," "," ","█"," "," "," ","█"," "," "," ","█"," ","█"," ","#","#"," "," ","█"," ","█"},
        {"█"," ","█"," ","█","█","█","█","█"," ","█"," ","█","█","█","█","█","█","█"," ","█"," ","█","█","█"," ","█","█","█","█","█"," ","█","█","█"," ","█"," ","█"," ","█","█","█","#","█","█","█","█","█"," ","█"},
        {"█","#","#","#","█"," "," "," "," "," ","█"," "," "," "," "," "," "," "," "," ","█"," "," ","#","#","#","█","#","#"," ","█"," "," "," "," "," ","█"," ","█"," "," "," ","█","#","#","#","#","#","#"," ","█"},
        {"█","c","█","█","█"," ","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","#","█"," ","█","█","█","█","█","█","█"," ","█","█","█"," ","█","#"," "," "," "," ","#"," ","█"},
        {"█","c","c","c","█"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ","#","#","#","#","#","#","█","#","#"," "," "," "," "," "," "," "," "," ","█"," "," "," "," ","#","#"," "," "," ","█"},
        {"█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█","█"}
    };

bool move_possible(int y, int x, int dir){
    if(dir == 0) return true;
    else if(dir == 1 && strcmp(map[y-1][x],"█") == 0)  return false;
    else if(dir == 2 && strcmp(map[y][x-1],"█") == 0)  return false;
    else if(dir == 3 && strcmp(map[y+1][x],"█") == 0)  return false;
    else if(dir == 4 && strcmp(map[y][x+1],"█") == 0)  return false;
    return true;
}
bool player_in_FOV(int pos_y, int pos_x){
    for(int y = -2; y <= 2; y++){
        for(int x = -2; x <= 2; x++){
            if(pos_y + y >= 0 && pos_y + y < MAP_SIZE_Y && pos_x + x >= 0 && pos_x + x < MAP_SIZE_X){
                if(strcmp(map[pos_y+y][pos_x+x],"1") == 0 ||strcmp(map[pos_y+y][pos_x+x],"2") == 0 || strcmp(map[pos_y+y][pos_x+x],"3") == 0 || strcmp(map[pos_y+y][pos_x+x],"4") == 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
bool wall_blocks_FOV(int pos_y, int pos_x){

    int player_y, player_x;
    bool break_double_loop = false;

    //get player position
    for(int y = -2; y <= 2; y++){
        for(int x = -2; x <= 2; x++){
            player_y = pos_y + y;
            player_x = pos_x + x;
            if(player_y >= 0 && player_y < MAP_SIZE_Y && player_x >= 0 && player_x < MAP_SIZE_X){
                if(strcmp(map[player_y][player_x],"1") == 0 ||strcmp(map[player_y][player_x],"2") == 0 || strcmp(map[player_y][player_x],"3") == 0 || strcmp(map[player_y][player_x],"4") == 0)
                {
                    break_double_loop = true;
                }
            }
            if(break_double_loop) break;
        }
        if(break_double_loop) break;
    }

    //check for blocked view
    int y_diff =  player_y - pos_y;
    int x_diff = player_x - pos_x;///////////////
    int diff_sum = y_diff + x_diff;
    if(strcmp(map_blueprint[pos_y + y_diff/2][pos_x + x_diff/2],"█") == 0) return true;
    if(diff_sum % 2 == 1 || diff_sum % 2 == -1)
    {
        int sec_wall_y = y_diff - y_diff/2;
        int sec_wall_x = x_diff - x_diff/2;
        if(strcmp(map_blueprint[sec_wall_y][sec_wall_x],"█") == 0) return true;
    }
    return false;

}
bool beast_can_see_player(int pos_y, int pos_x){
    //if player not in 5x5 area, return false
    if(!player_in_FOV(pos_y,pos_x)) return false;

    //if beast next to player(or on player), return true
    for(int y = -1; y <= 1; y++){
        for(int x = -1; x <= 1; x++){
            if(pos_y + y >= 0 && pos_y + y < MAP_SIZE_Y && pos_x + x >= 0 && pos_x + x < MAP_SIZE_X){
                if(strcmp(map[pos_y+y][pos_x+x],"1") == 0 ||strcmp(map[pos_y+y][pos_x+x],"2") == 0 || strcmp(map[pos_y+y][pos_x+x],"3") == 0 || strcmp(map[pos_y+y][pos_x+x],"4") == 0)
                {
                    return true;
                }
            }
        }
    }

    //if beast two fields from player, check if wall blocks view
    if(wall_blocks_FOV(pos_y,pos_x))
    {
        return false;
    }

    return true;

}

void* beast_create(void* arg){

    //create beast
    int pos_x, pos_y;
    bool found_free_space = false;
    pthread_mutex_lock(&map_mutex);
    while(!found_free_space){
        pos_x = rand()%49 + 1;
        pos_y = rand()%23 + 1;
        if(strcmp(map[pos_y][pos_x], " ") == 0 && strcmp(map_blueprint[pos_y][pos_x], " ") == 0) found_free_space = true;
    }
    strcpy(map[pos_y][pos_x], "*");
    pthread_mutex_unlock(&map_mutex);

    char sem_name[5];
    char sem2_name[6];
    snprintf(sem_name, 5, "b%d", beast_count);
    snprintf(sem2_name,6,"bb%d",beast_count);
    sem_t new_post_sem;
    sem_t new_get_sem;
    sem_init(&new_post_sem, 0, 0);
    sem_init(&new_get_sem,0,0);

    pthread_mutex_lock(&beasts_mutex);
    beasts_post_semaphores.push_back(&new_post_sem);
    beasts_get_semaphores.push_back(&new_get_sem);
    pthread_mutex_unlock(&beasts_mutex);

    //start beast
    bool choose_new_dir = true;
    int random_dir = 1;
    while(game_is_on)
    {

        sem_wait(&new_post_sem);////////////////////////////

        pthread_mutex_lock(&map_mutex);//////
        strcpy(map[pos_y][pos_x],map_blueprint[pos_y][pos_x]);
        pthread_mutex_unlock(&map_mutex);////

        if(beast_can_see_player(pos_y, pos_x))
        {
            int player_y, player_x;
            bool break_double_loop = false;

            //get player position
            for(int y = -2; y <= 2; y++){
                for(int x = -2; x <= 2; x++){
                    player_y = pos_y + y;
                    player_x = pos_x + x;
                    if(player_y >= 0 && player_y < MAP_SIZE_Y && player_x >= 0 && player_x < MAP_SIZE_X){
                        if(strcmp(map[player_y][player_x],"1") == 0 ||strcmp(map[player_y][player_x],"2") == 0 || strcmp(map[player_y][player_x],"3") == 0 || strcmp(map[player_y][player_x],"4") == 0)
                        {
                            break_double_loop = true;
                        }
                    }
                    if(break_double_loop) break;
                }
                if(break_double_loop) break;
            }
            int y_diff =  player_y - pos_y;
            int x_diff = player_x - pos_x;

            if(y_diff > 0 && x_diff == 0)
            {
                random_dir = 3;
            }
            else if(y_diff < 0 && x_diff == 0)
            {
                random_dir = 1;
            }
            else if(y_diff == 0 && x_diff > 0)
            {
                random_dir = 4;
            }
            else if(y_diff == 0 && x_diff < 0)
            {
                random_dir = 2;
            }
            else if(y_diff > 0 && x_diff > 0)
            {
                if(strcmp(map_blueprint[pos_y + 1][pos_x], "█") == 0) random_dir = 4;
                else random_dir = 3;
            }
            else if(y_diff > 0 && x_diff < 0)
            {
                if(strcmp(map_blueprint[pos_y + 1][pos_x], "█") == 0) random_dir = 2;
                else random_dir = 3;
            }
            else if(y_diff < 0 && x_diff > 0)
            {
                if(strcmp(map_blueprint[pos_y - 1][pos_x], "█") == 0) random_dir = 4;
                else random_dir = 1;
            }
            else if(y_diff < 0 && x_diff < 0)
            {
                if(strcmp(map_blueprint[pos_y - 1][pos_x], "█") == 0) random_dir = 2;
                else random_dir = 1;
            }
            else random_dir = 1;

            if     (random_dir == 1)    --(pos_y);
            else if(random_dir == 2)    --(pos_x);
            else if(random_dir == 3)    ++(pos_y);
            else if(random_dir == 4)    ++(pos_x);

            pthread_mutex_lock(&map_mutex);
            strcpy(map[pos_y][pos_x], "*");
            pthread_mutex_unlock(&map_mutex);

        }
        else{
            if(choose_new_dir) while (!move_possible(pos_y, pos_x, random_dir)) random_dir = rand() % 4 + 1;
            if(move_possible(pos_y, pos_x, random_dir))
            {   //continue going in old direction
                if     (random_dir == 1)    --(pos_y);
                else if(random_dir == 2)    --(pos_x);
                else if(random_dir == 3)    ++(pos_y);
                else if(random_dir == 4)    ++(pos_x);
                choose_new_dir = false;

                pthread_mutex_lock(&map_mutex);
                strcpy(map[pos_y][pos_x], "*");
                pthread_mutex_unlock(&map_mutex);
            }
            else
            {   //choose new direction to move in next iteration
                choose_new_dir = true;
                pthread_mutex_lock(&map_mutex);
                strcpy(map[pos_y][pos_x], "*");
                pthread_mutex_unlock(&map_mutex);
            }
        }

        sem_post(&new_get_sem);////////////////////////
    }

    sem_close(&new_post_sem);
    sem_close(&new_get_sem);
}
void draw_map(){
    for(int y = 0; y < 25; y++){
        for(int x = 0; x < 51; x++){
            mvprintw(y,x,map[y][x]);
        }
    }
    mvprintw(1, 53, "Server PID: %d",server_pid);
    mvprintw(2, 53, "Campsite X/Y: 23/11");
    mvprintw(3, 53, "Round number: %d",round_number);
    mvprintw(5, 53, "Parameter:     Player1 Player2 Player3 Player4");
    mvprintw(6, 53, "PID");
    mvprintw(7, 53, "Curr X/Y:");
    mvprintw(8, 53, "Death count:");
    mvprintw(9, 53, "Coins carried:");
    mvprintw(10, 53, "Coins brought:");
    for(int i = 0; i < 4; i++){
        mvprintw(6,60+(8*(i+1)),"-------");
        mvprintw(7,60+(8*(i+1)),"---/---");
        mvprintw(8,60+(8*(i+1)),"-------");
        mvprintw(9,60+(8*(i+1)),"-------");
        mvprintw(10,60+(8*(i+1)),"-------");
    }
    for(int i = 0; i < players.size(); i++){
        mvprintw(6,60+(8*(i+1)), "%7d",(int)players[i]->player_pid);
        mvprintw(7,60+(8*(i+1)), "  %02d/%02d",players[i]->pos_x, players[i]->pos_y);
        mvprintw(8,60+(8*(i+1)), "%7d",players[i]->death_count);
        mvprintw(9,60+(8*(i+1)), "%7d",players[i]->coins_carried);
        mvprintw(10,60+(8*(i+1)), "%7d",players[i]->coins_brought);
    }

    mvprintw(14,53, "Legend:");
    mvprintw(16,53, "1234 - players");
    mvprintw(17,53, "█ - wall");
    mvprintw(18,53, "# - bushes");
    mvprintw(19,53, "* - wild beast");
    mvprintw(20,53, "c - one coin");
    mvprintw(21,53, "t - treasure (10 coins)");
    mvprintw(22,53, "T - large treasure (50 coins)");
    mvprintw(23,53, "D - dropped treasure (dead body)");
    mvprintw(24,53, "A - campsite");

    refresh();
}
void add_new_player(Player* player){
    players.push_back(player);
    player->symbol_char[0] = (int)players.size() + '0';
    strcpy(map[player->pos_y][player->pos_x], player->symbol_char);
//    printf("\n\nAdded player\n\n");
}
bool player_closed_game(pid_t pid){
    return 0 != kill(pid,0);
}
void change_position(Player* player){

    if(player->direction != 0) {
        strcpy(map[player->pos_y][player->pos_x], map_blueprint[player->pos_y][player->pos_x]);
        player->player_waited = false;
    }
    if(player->direction == 0) player->player_waited = true;
    else if(player->direction == 1) --(player->pos_y);
    else if(player->direction == 2) --(player->pos_x);
    else if(player->direction == 3) ++(player->pos_y);
    else if(player->direction == 4) ++(player->pos_x);




    strcpy(map[player->pos_y][player->pos_x],player->symbol_char);
}
void init_screen(){
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    timeout(20);
    nonl();
    leaveok(stdscr,TRUE);
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
}
void reset_player(Player* player){
    player->pos_y = player->starting_y;
    player->pos_x = player->starting_x;
    player->player_waited = false;
    player->in_bush = false;
    player->direction = 0;
    player->coins_carried = 0;
    for(int y = 0; y < 25; y++){
        for(int x = 0; x < 51; x++){
            strcpy(player->his_pdata->map[y][x], "X");
            strcpy(player->player_map[y][x], "X");
            if(y - player->pos_y >= -2 && y - player->pos_y <= 2 && x - player->pos_x >= -2 && x - player->pos_x <= 2) player->player_map_allowed[y][x] = true;
            else player->player_map_allowed[y][x] = false;
        }
    }
    ++(player->death_count);
}

void* join(void* arg){

    Joiner join_manager;
    while(game_is_on){
        if(sem_trywait(join_manager.i_wanna_join_semaphore) == 0){
            sem_wait(&(join_manager.pjoin)->cs);/////////////////////////////////////////////////////////////////////////////////

            if(players.size() >=4) join_manager.pjoin->connection_refused = true;
            else join_manager.pjoin->connection_refused = false;

            Player* player = new Player(map); //create object representing player on server

            strcpy(join_manager.pjoin->name,player->name); //copy player name to join memory

            player->player_pid = join_manager.pjoin->player_pid; // remember player's pid


            sem_post(&(join_manager.pjoin)->cs);//////////////////////////////////////////////////////////////////////////////////
            sem_post(join_manager.get_your_data_semaphore); //wyslij sygnal by gracz odebral swoje dane///////////////////////////
            if(players.size() < 4) add_new_player(player);
        } else sleep(1);
    }
    for(auto player : players){
        sem_close(player->sem_send_move);
        sem_unlink(player->name);
        delete player;
    }
    printf("\nJoin th ended\n");
    return nullptr;
}
void* keep_connection(void* arg){
    //checks all players processes's PID's to see if they exist, if not, delete them from server
    while(game_is_on){
        for(auto it = players.begin(); it != players.end(); /* NOTHING */){
            if (player_closed_game((*it)->player_pid)){
                int tempx = (*it)->pos_x;
                int tempy = (*it)->pos_y;
                for(int i = 0; i < 25;i++){
                    for(int j = 0; j < 51; j++){
                        (*it)->player_map_allowed[i][j] = false;
                        strcpy((*it)->player_map[i][j], "X");
                    }
                }
                sem_close((*it)->sem_send_move);
                sem_unlink((*it)->name);
                delete *it;
                it = players.erase(it);
                strcpy(map[tempy][tempx], map_blueprint[tempy][tempx]);
            }
            else ++it;
        }

        sleep(1);
    }
    printf("\nkeep th ended\n");
}
void* game(void* arg){
    init_screen();
    while(game_is_on){
        round_number++;

        //get all players declared moves
        for(auto & player : players)
        {
            if (sem_trywait(player->sem_send_move) != -1)
            {
                sem_wait(&(player->his_pdata->cs));//////////////
                player->direction = player->his_pdata->direction;
                sem_post(&(player->his_pdata->cs));//////////////
                while(sem_trywait(player->sem_send_move) != -1); //clear excess signals
            }
            else{
                player->direction = 0;
            }
        }

        //apply player moves to the map
        for(auto & player : players)
        {
            if(move_possible(player->pos_y, player->pos_x, player->direction)){
                if(!player->in_bush || player->player_waited) change_position(player);
                else player->player_waited = true;

                //TRUTH TABLE UPDATING AFTER MOVE ////////////////
                for(int y = player->pos_y - 2; y <= player->pos_y + 2; y++){
                    for(int x = player->pos_x - 2; x <= player->pos_x + 2; x++){
                        if(y >= 0 && x >= 0 && y < MAP_SIZE_Y && x < MAP_SIZE_X){
                            player->player_map_allowed[y][x] = true;
                        }
                    }
                }
                //////////////////////////////////////////////////
            }
        }

        //apply beasts movement
        pthread_mutex_lock(&beasts_mutex);////////////
        for(auto & beasts_post_semaphore : beasts_post_semaphores){
            sem_post(beasts_post_semaphore);
        }
        pthread_mutex_unlock(&beasts_mutex);////////////
        pthread_mutex_lock(&beasts_mutex);////////////
        for(auto & beasts_get_semaphore : beasts_get_semaphores){
            sem_wait(beasts_get_semaphore);
        }
        pthread_mutex_unlock(&beasts_mutex);///////////

        //apply players collision
        for(auto it1 = players.begin(); it1 != players.end(); ++it1)
        {
            for(auto it2 = it1+1;it2 != players.end(); ++it2)
            {
                if((*it1)->pos_x == (*it2)->pos_x && (*it1)->pos_y == (*it2)->pos_y)
                {
                    struct treasure_t treasure;
                    treasure.x = (*it1)->pos_x;
                    treasure.y = (*it1)->pos_y;
                    treasure.value = 0;
                    treasure.value += (*it1)->coins_carried;
                    treasure.value += (*it2)->coins_carried;
                    treasures.push_back(treasure);


                    strcpy(map_blueprint[(*it1)->pos_y][(*it1)->pos_x], "D");
                    strcpy(map[(*it1)->pos_y][(*it1)->pos_x], "D");
                    reset_player((*it1));
                    reset_player((*it2));
                }
            }
        }

        //apply object collision
        for(auto & player: players)
        {
            if(strcmp(map_blueprint[player->pos_y][player->pos_x]," ") == 0)/*DO NOTHING*/;
            else if(strcmp(map_blueprint[player->pos_y][player->pos_x],"c") == 0)
            {
                strcpy(map_blueprint[player->pos_y][player->pos_x]," ");
                strcpy(map[player->pos_y][player->pos_x]," ");
                player->coins_carried += 1;
            }
            else if(strcmp(map_blueprint[player->pos_y][player->pos_x],"t") == 0)
            {
                strcpy(map_blueprint[player->pos_y][player->pos_x]," ");
                strcpy(map[player->pos_y][player->pos_x]," ");
                player->coins_carried += 10;
            }
            else if(strcmp(map_blueprint[player->pos_y][player->pos_x],"T") == 0)
            {
                strcpy(map_blueprint[player->pos_y][player->pos_x]," ");
                strcpy(map[player->pos_y][player->pos_x]," ");
                player->coins_carried += 50;
            }
            else if(strcmp(map_blueprint[player->pos_y][player->pos_x],"A") == 0)
            {
                player->coins_brought += player->coins_carried;
                player->coins_carried = 0;
            }
            else if(strcmp(map_blueprint[player->pos_y][player->pos_x],"D") == 0)
            {
                for(auto & treasure : treasures){
                    if(treasure.x == player->pos_x && treasure.y == player->pos_y){
                        player->coins_carried += treasure.value;
                    }
                }
                strcpy(map_blueprint[player->pos_y][player->pos_x],map_for_treasures[player->pos_y][player->pos_x]);
            }

            if(strcmp(map_blueprint[player->pos_y][player->pos_x],"#") == 0)
            {
                player->in_bush = true;
            }
            else {
                player->in_bush = false;
            }

            if(strcmp(map[player->pos_y][player->pos_x],"*") == 0)
            {
                struct treasure_t treasure;
                treasure.x = player->pos_x;
                treasure.y = player->pos_y;
                treasure.value = 0;
                treasure.value += player->coins_carried;
                treasures.push_back(treasure);

                strcpy(map_blueprint[player->pos_y][player->pos_x], "D");
                strcpy(map[player->pos_y][player->pos_x], "D");
                reset_player(player);
            }
        }

        //update players map
        for(auto & player : players)
        {
            for(int y = 0; y < MAP_SIZE_Y; y++){
                for(int x = 0; x < MAP_SIZE_X; x++){
                    if(player->player_map_allowed[y][x]){
                        strcpy(player->player_map[y][x], map[y][x]);
                    }
                }
            }
        }

        //send data to players
        for(auto & player : players)
        {
            sem_wait(&player->his_pdata->cs);/////////////////////////////////////////

            for(int y = 0; y < MAP_SIZE_Y; ++y){
                for(int x = 0; x <MAP_SIZE_X; ++x){
                    strcpy(player->his_pdata->map[y][x],player->player_map[y][x]);
                }
            }

            player->his_pdata->coins_carried = player->coins_carried;
            player->his_pdata->coins_brought = player->coins_brought;
            player->his_pdata->death_count = player->death_count;
            player->his_pdata->pos_y = player->pos_y;
            player->his_pdata->pos_x = player->pos_x;
            player->his_pdata->round_number = round_number;

            sem_post(&player->his_pdata->cs);//////////////////////////////////////////
        }

        draw_map();

        int key = wgetch(stdscr);
        if (key == 'Q' || key == 'q') game_is_on = false;
        else if(key == 'c'){
            int pos_x, pos_y;
            bool found_free_space = false;
            while(!found_free_space){
                pos_x = rand()%49 + 1;
                pos_y = rand()%23 + 1;
                if(strcmp(map[pos_y][pos_x], " ") == 0 && strcmp(map_blueprint[pos_y][pos_x], " ") == 0) found_free_space = true;
            }
            strcpy(map[pos_y][pos_x], "c");
            strcpy(map_blueprint[pos_y][pos_x], "c");
        }
        else if(key == 't'){
            int pos_x, pos_y;
            bool found_free_space = false;
            while(!found_free_space){
                pos_x = rand()%49 + 1;
                pos_y = rand()%23 + 1;
                if(strcmp(map[pos_y][pos_x], " ") == 0 && strcmp(map_blueprint[pos_y][pos_x], " ") == 0) found_free_space = true;

            }
            strcpy(map[pos_y][pos_x], "t");
            strcpy(map_blueprint[pos_y][pos_x], "t");
        }
        else if(key == 'T'){
            int pos_x, pos_y;
            bool found_free_space = false;
            while(!found_free_space){
                pos_x = rand()%49 + 1;
                pos_y = rand()%23 + 1;
                if(strcmp(map[pos_y][pos_x], " ") == 0 && strcmp(map_blueprint[pos_y][pos_x], " ") == 0) found_free_space = true;

            }
            strcpy(map[pos_y][pos_x], "T");
            strcpy(map_blueprint[pos_y][pos_x], "T");
        }
        else if(key == 'b' || key == 'B'){
            pthread_t beast_th;
            pthread_create(&beast_th,NULL,beast_create,NULL);
            ++beast_count;

        }
        usleep(300000);
    }
    endwin();
    printf("\ngame th ended\n");
}

int main() {
    srand(time(NULL));

    pthread_t th_join, th_game, th_keep_connection, th_beast;

    pthread_create(&th_join, NULL, join, NULL);
    pthread_create(&th_game, NULL, game, NULL);
    pthread_create(&th_keep_connection, NULL, keep_connection, NULL);

    pthread_create(&th_beast,NULL,beast_create,NULL);
    ++beast_count;

    pthread_join(th_game,NULL);
    pthread_join(th_keep_connection,NULL);
    pthread_join(th_join,NULL);

    return 0;
}
