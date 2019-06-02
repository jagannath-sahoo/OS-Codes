#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

struct shr_mem{
    unsigned long count;
    char temp[100];
};

struct shr_mem *shma;
pthread_mutex_t mutex_sh;
pthread_t th1,th2;
pthread_attr_t attr1,attr2;

void *thread_1(void *args)
{
    unsigned long i = 0;
    while(i++<100000000)
    {
        pthread_mutex_lock(&mutex_sh);
        shma->count++;
        pthread_mutex_unlock(&mutex_sh);
    }
}

void *thread_2(void *args)
{
        unsigned long i = 0;
    while(i++<10000000)
    {
        pthread_mutex_lock(&mutex_sh);
        shma->count--;
        pthread_mutex_unlock(&mutex_sh);
    }
}

int main()
{
    int ret;
    void *ptr = NULL;
    //initialization of shared memory
    int id = shmget(1141,sizeof(struct shr_mem),IPC_CREAT|0600);
    if(id< 0)
    {
        perror("Error in creation of shared mem id");
        exit(EXIT_FAILURE);
    }
    //id: shared memory id
    //addr: Allocate from any location
    //flag: 0
    shma = shmat(id,0,0);
    if(shma == (struct shr_mem *)-1)
    {
        perror("Error in creating shared memory");
        exit(EXIT_FAILURE);
    }
    shma->count = 1;
    strcpy(shma->temp,"HELLO");

    ret = pthread_mutex_init(&mutex_sh,NULL);
    if(ret != 0)
    {
        perror("Error in initialization of mutex");
        exit(EXIT_FAILURE);
    }
    ptr = malloc(32*1024);
    if(ptr != 0)
    {
        ret = pthread_attr_init(&attr1);
        if(ret != 0)
        {
            perror("Error in setting initialization of attribute of threa 1");
            exit(EXIT_FAILURE);
        }
        if(pthread_attr_setstack(&attr1, ptr, 30*1024) != 0)
        {
            perror("Error in setting stack size thread1");
            exit(EXIT_FAILURE);
        }
    }
    ret = pthread_create(&th1,&attr1,thread_1,NULL);
    if(ret != 0)
    {
        perror("Error in creating thread");
        exit(EXIT_FAILURE);
    }
    ret = pthread_create(&th2,NULL,thread_2,NULL);
    if(ret != 0)
    {
        perror("Error in creating thread");
        exit(EXIT_FAILURE);
    }
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);

    printf("Final Value: %ld",shma->count);
    return 0;
}