#ifndef PROJECT3_PLAYER_H
#define PROJECT3_PLAYER_H


class Player {
public:
    sem_t* semaphore;
    int shm_handle;
    struct player_t* shm_player_data;
    char name[100];
    char map[25][51][4];
    int pos_y;
    int pos_x;
    int map_size_y;
    int map_size_x;
    int coins_carried;
    int coins_brought;
    pid_t server_pid;
    int death_count;
    int round_number;

    Player(char* name, pid_t server_pid){
        coins_carried = 0;
        semaphore = sem_open(name,0);
        shm_handle = shm_open(name, O_CREAT | O_RDWR, 0666);
        shm_player_data = (struct player_t*) mmap(NULL, sizeof(struct player_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_handle, 0);
        map_size_y = shm_player_data->map_size_y;
        map_size_x = shm_player_data->map_size_x;
        this->server_pid = server_pid;
        death_count = 0;
    }
    Player( const Player & object );
    ~Player(){
        close(shm_handle);
        shm_unlink(name);
        sem_close(semaphore);
        sem_unlink(name);
        printf("\nPlayer destructor activated\n");
    }
};
class Joiner {
public:
    sem_t* join_semaphore;
    sem_t* get_data_semaphore;
    int shm_handle;
    struct join_t* pjoin;

    Joiner(){
//        printf("\nI was here!\n");
        join_semaphore = sem_open(I_WANNA_JOIN_SEMAPHORE, 0);
//        printf("\nI was here!\n");
        get_data_semaphore = sem_open(GET_YOU_DATA_SEMAPHORE,0);
//        printf("\nI was here!\n");
        shm_handle = shm_open(JOIN_SHARED_MEM_NAME, O_RDWR, 0666);
//        printf("\nI was here!\n");
        pjoin = (struct join_t*) mmap(NULL, sizeof(struct join_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_handle, 0);
//        printf("\nI was here!\n");
    }
    ~Joiner(){
        munmap(pjoin, sizeof(struct join_t));
    }
    Joiner( const Joiner & object );

    bool server_off(){
        printf("\nI was here\n");
        printf("%d",pjoin->player_pid);
        if (0 != kill(pjoin->server_pid, 0)){
            return true;
        }
    }
    void sent_join_request(){
        sem_wait(&pjoin->cs);//////////

        pjoin->player_pid = getpid();

        sem_post(&pjoin->cs);/////////
//        printf("\nI was here!\n");
        sem_post(join_semaphore);
//        printf("\nI was here!\n");
    }
    bool wait_for_answer(){
        sem_wait(get_data_semaphore);
//        printf("\n\nGot anwear!\n");
        sem_wait(&pjoin->cs);

        if(pjoin->connection_refused){
            sem_post(&pjoin->cs);
//            printf("Odmawiam\n");
//            while(true);
            return true;
        }
        return false;
    }

};

#endif //PROJECT3_PLAYER_H
