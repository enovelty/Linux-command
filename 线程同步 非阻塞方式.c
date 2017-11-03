#include <stdio.h>
#include <pthread.h>
#include <unistd.h>		//sleep

void man(int *);

pthread_mutex_t mutex[6];

int main(){
	pthread_t man1, man2, man3, man4, man5;
	int a = 1, b = 2, c = 3, d = 4, e = 5;
	
	pthread_create(&man1, NULL, (void *)man, &a);
	pthread_create(&man2, NULL, (void *)man, &b);
	pthread_create(&man3, NULL, (void *)man, &c);
	pthread_create(&man4, NULL, (void *)man, &d);
	pthread_create(&man5, NULL, (void *)man, &e);
	
	pthread_join(man1, NULL);
	pthread_join(man2, NULL);
	pthread_join(man3, NULL);
	pthread_join(man4, NULL);
	pthread_join(man5, NULL);
	
	return 0;
}

void man(int *n){
	int busy;	
	while(1){
		printf("man %d is thinking.\n", *n);
		sleep(1);
		
		pthread_mutex_lock(&mutex[*n]);

		if (*n == 5)
			busy = pthread_mutex_trylock(&mutex[1]);
		else
			busy = pthread_mutex_trylock(&mutex[*n+1]);
		
		if (busy){
			pthread_mutex_unlock(&mutex[*n]);
			continue;
		}
		
		printf("man %d is eating.\n", *n);
		
		pthread_mutex_unlock(&mutex[(*n)]);
		if (*n == 5)
			pthread_mutex_unlock(&mutex[1]);
		else
			pthread_mutex_unlock(&mutex[*n+1]);
	}//while
}


