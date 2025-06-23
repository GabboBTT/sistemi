#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <time.h>

// ricordati di eseguire con -lpthread

	int sem1, sem2;
	uint64_t buffer1, buffer2, s = 1;
	int r;
	

void* ciao ( void *vargp) {
	
	srand(time(NULL)); 
	
	while(1) {

		read(sem1, &buffer1, sizeof(uint64_t));
		r = rand() %20; 
		printf("sono il produttore ed ho fatto questo numero: %d\n ma ora credo che dormirò un po'\n",r);
		sleep(rand() %5);
		write(sem2, &s, sizeof(uint64_t));	

	}
	
	return NULL;
		
}

void* ciao2 ( void *vargp) {

	while(1) {
		read(sem2, &buffer2, sizeof(uint64_t));
		printf("sono il consumatore ed ho ricevuto questo numero: %d, ne aspetto un altro\n", r);
		sleep(rand() %5);
		write(sem1, &s, sizeof(uint64_t));
		
	}

	return NULL;

}

int main () {

	pthread_t id[2];
	sem1 = eventfd(1,EFD_SEMAPHORE);
	sem2 = eventfd(0,EFD_SEMAPHORE);
	pthread_create(&id[1], NULL, ciao2, NULL);
	pthread_create(&id[0], NULL, ciao, NULL);
	pthread_join(id[1], NULL);
	pthread_join(id[0], NULL);


	return 0;

}
