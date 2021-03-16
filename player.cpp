#include "common.h"
#include "player.h"

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
void get_data(Player* player){
    sem_wait(&player->shm_player_data->cs);///////////////////////////////////////
    for(int y = 0; y < player->map_size_y; y++){
        for(int x = 0; x < player->map_size_x; x++){
            strcpy(player->map[y][x], player->shm_player_data->map[y][x]);
        }
    }
    player->coins_carried = player->shm_player_data->coins_carried;
    player->coins_brought = player->shm_player_data->coins_brought;
    player->death_count = player->shm_player_data->death_count;
    player->pos_x = player->shm_player_data->pos_x;
    player->pos_y = player->shm_player_data->pos_y;
    player->round_number = player->shm_player_data->round_number;
    sem_post(&player->shm_player_data->cs);///////////////////////////////////////
}
void draw_map(Player* player){

    for(int y = 0; y < player->map_size_y; y++){
        for(int x = 0; x < player->map_size_x; x++){
            mvprintw(y,x,player->map[y][x]);
        }
    }
    mvprintw(0,52,"Server PID: %d",player->server_pid);
    mvprintw(1,52,"Coins carried: %3d",player->coins_carried);
    mvprintw(2,52,"Coins brought: %3d",player->coins_brought);
    mvprintw(3,52,"Death count: %3d",player->death_count);
    mvprintw(4,52,"Player X: %2d Player Y: %2d",player->pos_x, player->pos_y);
    mvprintw(5,52,"Round number: %d",player->round_number);

    refresh();
}
void send_data(Player* player, int direction){
    if(direction >= 1 && direction <= 4){
        sem_wait(&player->shm_player_data->cs);
        player->shm_player_data->direction = direction;
        sem_post(&player->shm_player_data->cs);
        sem_post(player->semaphore);
    }
}
int main(){

    init_screen();

    Joiner join_manager = Joiner();
    if (join_manager.server_off()){
        printf("\nGame server is off, try again later\n");
        return 0;
    }

    join_manager.sent_join_request();

//    printf("\nI was here!\n");
    bool connection_refused = join_manager.wait_for_answer();

    if(connection_refused){
        printf("\nMax num of players reached, game won't commence\n");
        endwin();
        return 0;
    }

    Player player(join_manager.pjoin->name, join_manager.pjoin->server_pid);

    sem_post(&join_manager.pjoin->cs);







    // player connected, from now on, use my_shm_handle, shm_player_data and my_own_semaphore
    while(true){
        if(0 != kill(player.server_pid,0)) break;
        get_data(&player);
        draw_map(&player);

        int key = wgetch(stdscr);
        if(key == ERR) {usleep(100000); continue; }
        flushinp();

        if(key == 'q' || key == 'Q')break;


        int direction = -1;
        if(key == KEY_UP) direction = 1;
        else if(key == KEY_LEFT) direction = 2;
        else if(key == KEY_DOWN) direction = 3;
        else if(key == KEY_RIGHT) direction = 4;

        send_data(&player,direction);


        usleep(100000);
    };
    endwin();
    return 0;
}