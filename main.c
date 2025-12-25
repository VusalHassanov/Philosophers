#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *foo(void *arg){
	pthread_t threadId;
	pthread_t *thread2Ptr;
	
	threadId = pthread_self();
	thread2Ptr = (pthread_t *)arg;
	printf("thread1 ID: %lu\n", (unsigned long)threadId);
	printf("Waiting for thread2 to finish...\n");
	pthread_join(*thread2Ptr, NULL);
	printf("thread1 finished\n");
	pthread_exit(NULL);//Terminates thread
	printf("Will not run");//doesn't reach
	return NULL;
}

void *printLoop(void *arg){
	pthread_t threadId = pthread_self();

	sleep(1);
	printf("thread2 ID: %lu\n", (unsigned long)threadId);
	while(1){
		printf("Crazy print loop \n");
		sleep(1);
	}
}

int main(){
	pthread_t thread1;
	pthread_t thread2;

	if(pthread_create(&thread2, NULL, printLoop, &thread1)){
		printf("thread2 failed\n");
		return 1;
	}
	if(pthread_create(&thread1, NULL, foo, &thread2) != 0){
		printf("Thread failed\n");
		return 1;
	}
	sleep(5);
	pthread_cancel(thread2);
	printf("thread2 finished \n");
	pthread_join(thread1, NULL);
	printf("Main thread finished\n");
	return 0;
}