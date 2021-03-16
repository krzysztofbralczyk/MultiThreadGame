

#ifndef PROJECT3_SERVER_H
#define PROJECT3_SERVER_H

#define MAP_SIZE_Y 25
#define MAP_SIZE_X 51

class Player{
public:
    char name[100]; //needed for semaphore and shared memory
    sem_t* sem_send_move;
    int his_shm_handle;
    struct player_t* his_pdata;
    pid_t player_pid;
    int pos_x;
    int pos_y;
    int starting_x;
    int starting_y;
    char symbol_char[2];
    int direction;
    char player_map[25][51][4];
    bool player_map_allowed[25][51];
    int coins_carried;
    int coins_brought;
    bool in_bush;
    bool player_waited;
    int death_count;

    Player(char (*map)[51][4]){
//        printf("\nCreated player object\n");
        death_count = 0;
        in_bush = false;
        coins_carried = 0;
        coins_brought = 0;
        pos_x = rand()%49 + 1;
        int found_free_space = false;
        while(!found_free_space){
            pos_y = rand()%23 + 1;
            if(strcmp(map[pos_y][pos_x], " ") == 0) found_free_space = true;
        }
        starting_x = pos_x;
        starting_y = pos_y;
        symbol_char[1] = '\0';
        std::string s = std::to_string(rand()%10000);
        char const *pchar = s.c_str();
        strcpy(name,pchar);
        sem_send_move = sem_open(name, O_CREAT, 0666, 0);
        his_shm_handle = shm_open(name, O_CREAT | O_RDWR, 0666);
        ftruncate(his_shm_handle, sizeof(struct player_t));
        his_pdata = (struct player_t*) mmap(NULL,sizeof(struct player_t), PROT_READ | PROT_WRITE, MAP_SHARED, his_shm_handle, 0);
        sem_init(&(his_pdata->cs), 1, 1);
        for(int i = 0; i < 25; i++){
            for(int j = 0; j < 51; j++){
                strcpy(his_pdata->map[i][j], "X");
            }
        }
        his_pdata->map_size_x = MAP_SIZE_X;
        his_pdata->map_size_y = MAP_SIZE_Y;
        direction = 0;
        for(int y = 0; y < 25; y++){
            for(int x = 0; x < 51; x++){
                if(y - pos_y >= -2 && y - pos_y <= 2 && x - pos_x >= -2 && x - pos_x <= 2) player_map_allowed[y][x] = true;
                else player_map_allowed[y][x] = false;
            }
        }
    }
    Player( const Player & object );
    ~Player(){
        munmap(his_pdata, sizeof(struct player_t));
        close(his_shm_handle);
        shm_unlink(name);
        sem_close(sem_send_move);
//        printf("\nPlayer destructor called");
//        printf("%s\n",symbol_char);
    }
};
class Joiner{
public:
    sem_t* get_your_data_semaphore;
    sem_t* i_wanna_join_semaphore;
    int sh_memory_handle;
    struct join_t* pjoin;
//    bool server_already_on;

    Joiner(){
//        server_already_on = false;
//        i_wanna_join_semaphore = sem_open(I_WANNA_JOIN_SEMAPHORE, O_CREAT, 0666, 0);
//        get_your_data_semaphore = sem_open(GET_YOU_DATA_SEMAPHORE, O_CREAT, 0666, 0);
//        sh_memory_handle = shm_open(JOIN_SHARED_MEM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
//
//
//        if(sh_memory_handle == -1) {
//
//            sh_memory_handle = shm_open(JOIN_SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
//            ftruncate(sh_memory_handle, sizeof(struct join_t));
//            pjoin = (struct join_t*) mmap(NULL, sizeof(struct join_t), PROT_READ | PROT_WRITE, MAP_SHARED, sh_memory_handle, 0);
//            int kill_result = kill(pjoin->server_pid,0);
//            if(pjoin->server_pid == 0 || kill_result != 0){
//                //pozbadz sie smieci
//                munmap(pjoin, sizeof(struct join_t));
//                close(sh_memory_handle);
//                shm_unlink(JOIN_SHARED_MEM_NAME);
//                sem_close(i_wanna_join_semaphore);
//                sem_close(get_your_data_semaphore);
//
//                //otworz ponownie
//                i_wanna_join_semaphore = sem_open(I_WANNA_JOIN_SEMAPHORE, O_CREAT, 0666, 0);
//                get_your_data_semaphore = sem_open(GET_YOU_DATA_SEMAPHORE, O_CREAT, 0666, 0);
//                sh_memory_handle = shm_open(JOIN_SHARED_MEM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
//            }
//            else server_already_on = true;
//
//        }
//        //kontynuuj otwieranie
//        if(!server_already_on){
//            ftruncate(sh_memory_handle, sizeof(struct join_t));
//            pjoin = (struct join_t*) mmap(NULL, sizeof(struct join_t), PROT_READ | PROT_WRITE, MAP_SHARED, sh_memory_handle, 0);
//            pjoin->connection_refused = false;
//            pjoin->server_pid = getpid();
//            sem_init(&(pjoin->cs), 1, 1);
//        }

        i_wanna_join_semaphore = sem_open(I_WANNA_JOIN_SEMAPHORE, O_CREAT, 0666, 0);
        get_your_data_semaphore = sem_open(GET_YOU_DATA_SEMAPHORE, O_CREAT, 0666, 0);
        sh_memory_handle = shm_open(JOIN_SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
        ftruncate(sh_memory_handle, sizeof(struct join_t));
        pjoin = (struct join_t*) mmap(NULL, sizeof(struct join_t), PROT_READ | PROT_WRITE, MAP_SHARED, sh_memory_handle, 0);
        pjoin->connection_refused = false;
        pjoin->server_pid = getpid();
        sem_init(&(pjoin->cs), 1, 1);

    }
    Joiner( const Joiner & object );
    ~Joiner(){
        sem_close(i_wanna_join_semaphore);
        sem_close(get_your_data_semaphore);
        sem_unlink(I_WANNA_JOIN_SEMAPHORE);
        sem_unlink(GET_YOU_DATA_SEMAPHORE);
        munmap(pjoin, sizeof(struct join_t));
        close(sh_memory_handle);
        shm_unlink(JOIN_SHARED_MEM_NAME);
//        printf("Joiner destructor called\n");
    }


};


struct treasure_t {
    int value;
    int y;
    int x;
};
#endif //PROJECT3_SERVER_H
