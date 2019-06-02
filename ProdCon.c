#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define KEY1	1441
#define KEY2	9452


typedef struct shared_mem_area{
	int rd_index;
	int wr_index;
	int buf_max_size;
	int used_slot_count;
	char buff[5][256];
}SHARED_MEM_AREA_st;
SHARED_MEM_AREA_st *shared_mem;


union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

int main()
{
	int ret;
	union semun u1;
	struct sembuf sb1, sb2, sb3;
	//1st Semaphore for counting free slots
	//2nd for Mutual Exclusive access
	//3rd for Counting filled slots
	unsigned short ary[] = {5,1,0};
/*********************************************************************/
	/*
	 *
	 * Create shared memory id
	 * */
	int shm_id = shmget(KEY1, 5*sizeof(SHARED_MEM_AREA_st), IPC_CREAT|0600);
	if(shm_id < 0)
	{
		perror("Error in creating shared memory ");
	}

	//Create Virtual shared memory
	shared_mem = shmat(shm_id, 0, 0);

	if(shared_mem == (SHARED_MEM_AREA_st*)-1)
	{
		perror("Error in creation of shared memory object");
		exit(EXIT_FAILURE);
	}
	/***********************************************************/
	return 0;
}

