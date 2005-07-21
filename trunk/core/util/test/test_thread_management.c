#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <thread_management.h>

void *
test_thread(void * args) {

    printf("test_thread entered\n");
    
    sleep(2);
    printf("leaving test_thread\n");
    
    thread_terminated(pthread_self());
    pthread_exit(NULL);
}

void *
test_thread2(void * args) {

    int i;
    printf("test_thread2 entered\n");
    
    i = (int) args;
    printf("i is: %d\n", i);
    sleep(5);
    printf("leaving test_thread2\n");

    thread_terminated(pthread_self());
    pthread_exit(NULL);
}

int 
main() {
    int i = 2842;
    int x;
    tm_init();
    start_thread(test_thread, NULL);
    start_thread(test_thread2, (void *)i);
    scanf("%d", &x);
    start_thread(test_thread, NULL);
    scanf("%d", &x);
    tm_destroy();
    pthread_exit(NULL);

}
