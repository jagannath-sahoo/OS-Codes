#include<pthread.h>
#include<signal.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define KEY 1234


void *thread1(void *arg)
{
	while(1)
	{
	}
	pthread_exit(NULL);
}

void *thread2(void *arg)
{
	while(1)
	{
	}
	pthread_exit(NULL);
}

void *thread3(void *arg)
{
	while(1)
	{
	}
	pthread_exit(NULL);
}

void *thread4(void *arg)
{
	while(1)
	{
	}
	pthread_exit(NULL);
}

pthread_t thd1,thd2,thd3,thd4;
int main()
{
	int ret;
	void *ptr=NULL;
	struct sched_param param1;
	pthread_attr_t pthr;

	ptr=malloc(32*1024);
//	if(ret>0)
	{
		ret=pthread_attr_init(&pthr);
		if(ret>0)
		{
			perror("Error in thread attribute initialising\n");
			exit(5);
		}

		ret=pthread_attr_setstack(&pthr,ptr,30*1024);
		if(ret>0)
		{
			perror("Error in setting the attribute stack\n");
			exit(6);
		}
		pthread_attr_setschedpolicy(&pthr,SCHED_FIFO);

		param1.sched_priority=10;
		pthread_attr_setschedparam(&pthr,&param1);
	}



	ret=pthread_create(&thd1,&pthr,thread1,NULL);
	if(ret>0)
	{
		perror("Error in creating thread1\n");
		exit(1);
	}

	ret=pthread_create(&thd2,NULL,thread2,NULL);
	if(ret>0)
	{
		perror("Error in creating thread2\n");
		exit(2);
	}
	
	ret=pthread_create(&thd3,NULL,thread3,NULL);
	if(ret>0)
	{
		perror("Error in creating thread3\n");
		exit(3);
	}
	
	ret=pthread_create(&thd4,NULL,thread4,NULL);
	if(ret>0)
	{
		perror("Error in creating thread4\n");
		exit(4);
	}

	pthread_join(thd1,NULL);
	pthread_join(thd2,NULL);
	pthread_join(thd3,NULL);
	pthread_join(thd4,NULL);
}