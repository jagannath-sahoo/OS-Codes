#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define KEY 12345
#define MAX_SIZE    5
struct share_mem
{
    unsigned int wr_index;
    unsigned int rd_index;
    unsigned int used_slot;
    unsigned int buff_max_size;
    char buff[5][256];
};

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

struct share_mem *shr_mem;

int main()
{
    struct sembuf sb1,sb2,sb3;
    /*
    3 Semaphores
    1: counting free slots
    2: counting filled slots
    3: Exclusive access
    */
   int shm_mem_id = shmget(KEY,sizeof(struct share_mem), IPC_CREAT|0600);
   if(shm_mem_id < 0)
   {
       perror("Error in shm id create");
       exit(EXIT_FAILURE);
   }
    shr_mem = shmat(shm_mem_id,0,0);
    if(shr_mem == (struct share_mem *)-1)
    {
        perror("Error in shared memory attach");
        exit(EXIT_FAILURE);
    }

    int sema_id =  semget(KEY,3,IPC_CREAT|0600);
    if(sema_id < 0)
    {
        perror("Error in Semaphores id creation");
        exit(EXIT_FAILURE);
    }

    union semun u1;
    u1.val = 5;
    int ret = semctl(sema_id,0,SETVAL,u1);
    if(ret < 0)
    {
        perror("Error in Semaphore 0");
        exit(EXIT_FAILURE);
    }
    u1.val = 0;
    int ret = semctl(sema_id,1,SETVAL,u1);
    if(ret < 0)
    {
        perror("Error in Semaphore 1");
        exit(EXIT_FAILURE);
    }
    u1.val = 1;
    int ret = semctl(sema_id,2,SETVAL,u1);
    if(ret < 0)
    {
        perror("Error in Semaphore 2");
        exit(EXIT_FAILURE);
    }

    shr_mem->rd_index = 0;
    shr_mem->wr_index = 0;
    shr_mem->buff_max_size = MAX_SIZE;
    shr_mem->used_slot = 0;
    char *user_input;
    user_input = (char *) calloc(256,sizeof(char));
    ret = fork();
    while(1)
    {
        if(ret < 0)//Error
        {
            perror("Unable to allocate resource");
            exit(EXIT_FAILURE);
        }
        if(ret > 0)//Parent
        {
            scanf("%s",user_input);
            sb3.sem_num = 0;
            sb3.sem_op = -1;
            sb3.sem_flg = 0;
            semop(sema_id,&sb3,2);

            if(shr_mem->used_slot < shr_mem->buff_max_size)
            {
                strcpy(shr_mem->buff[shr_mem->wr_index],user_input);
                shr_mem->wr_index++;
                shr_mem->used_slot++;
            }
            sb1.sem_num = 0;
            sb1.sem_op = -1;
            sb1.sem_flg = 0;
            semop(sema_id,&sb1,2);
            sb2.sem_num = 0;
            sb2.sem_op = 1;
            sb2.sem_flg = 0;
            semop(sema_id,&sb2,2);
        }
        if

    }
    exit(EXIT_SUCCESS)
}