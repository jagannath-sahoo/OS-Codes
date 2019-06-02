#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define KEY 1441
#define KEY1	5541
union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

typedef struct shared_memory{
	unsigned long a;
	unsigned long b;
	unsigned long c;
}SHARED_MEM_AREA_st;

SHARED_MEM_AREA_st *shared_mem;


int main()
{
	int sem_id;
	union semun u1;
	unsigned long counter = 0;
	struct sembuf sops[2];
	/*
	 * Get the shared memory id 
	 * KEY: For Identify shared memory
	 * nsem: 1
	 * user permission
	 * */
	sem_id = semget(KEY, 1, IPC_CREAT | 0600);
	if(sem_id < 0)
	{
		perror("Error in sem-memory id");
		exit(EXIT_FAILURE);
	}

	u1.val = 0;
	/*
	 * Control Operation on Semaphores
	 * */
	int ret = semctl(sem_id, 0, SETVAL, u1);
	if(ret < 0)
	{
		perror("Error on Semaphore Object");
		exit(EXIT_FAILURE);
	}
	int shared_mem_id = shmget(KEY1, 3*1024, IPC_CREAT|0600);
	if(shared_mem_id < 0)
	{
		perror("Error in initialize of shared memory id");
		exit(EXIT_FAILURE);
	}

	/*
	 * shared_mem_id: shared memory id
	 * allocate from random location
	 * read only
	 * */
	shared_mem = shmat(shared_mem_id, 0,0);

	shared_mem->a = 1;
	shared_mem->b = 2;
	shared_mem->c = 3;

/********Complied ok***/
	//parent create child process
	ret = fork();
	if(ret < 0)
	{
		perror("Failed to allocate resource!");
		exit(EXIT_FAILURE);
	}
	if(ret > 0)//Parent process
	{
		while(counter++ < 100000000)
		{
			printf("Parent\n");
			printf("%ld\n",shared_mem->a);
        	        printf("%ld\n",shared_mem->b);
	                printf("%ld\n",shared_mem->c);
			//Decrement
			sops[0].sem_num = 0;        /* Operate on semaphore 0 */
	         	sops[0].sem_op = 0;         /* Wait for value to equal 0 */
        		sops[0].sem_flg = 0;
		}
		exit(EXIT_SUCCESS);
	}
	if(ret == 0)//Child process
	{
		while(counter++<100000000)
		{

			printf("CHILD\n");
			//Increment
			sops[0].sem_num = 0;        /* Operate on semaphore 0 */
		        sops[0].sem_op = 1;         /* Wait for value to equal 0 */
           		sops[0].sem_flg = 0;

			shared_mem->a++;
			shared_mem->b++;
			shared_mem->c++;
		}
		exit(EXIT_SUCCESS);
	}
	/*******Compile OK****/

	int status;
	if(ret > 0)
	{
		while(1)
		{
			ret = waitpid(-1,&status,0);
			if(ret > 0)
			{
				if(WIFEXITED(status))
				{
					if(WEXITSTATUS(status) == 0)
					{
						printf("SUCCESS: termination: %d",WEXITSTATUS(status));
					}	
					else
					{
						printf("UNSUCCESS: termination: %d",WEXITSTATUS(status));
					}

				}
			}
		}
	}
	return 0;
}
