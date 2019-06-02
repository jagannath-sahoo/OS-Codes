#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread1_aman(void *fd)
{
	int buff[1024];
	int ret = 0;
	int thd_fd = *((int *)fd);
	printf("Reading thread\n");
	ret = lseek(thd_fd, 0 ,SEEK_SET);
	if(ret != 0)
	{
		perror("Error in LSEEK");
		exit(1);
	}
	ret = read(thd_fd, buff, 30);
	if(ret > 0 )
	{
		write(STDOUT_FILENO, buff,ret);
	}
}

void thread2_str(void *fd);
pthread_t thd1, thd2;
//int fd;
int main()
{
	int ret;
	int fd;
	fd = open("/sys/kernel/kset-example4/aman/aman",O_RDWR);
	if(fd == -1)
	{
		perror("Error in opening");
		exit(1);
	}
	ret = pthread_create(&thd1,NULL,thread1_aman, (void *)&fd);
	if(ret != 0)
	{
		perror("error in creating thread");
		exit(1);
	}
	pthread_join(thd1,NULL);
	exit(1);
}

