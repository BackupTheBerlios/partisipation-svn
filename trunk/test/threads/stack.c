#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NTHREADS 4
#define N 1000
#define MEGEXTRA 1000000
 
pthread_attr_t attr;
 
void *dowork(void *threadid)
{
   double A[N][N];
   int i,j;
   size_t mystacksize;
 
   pthread_attr_getstacksize (&attr, &mystacksize);
   printf("Thread %d: stack size = %d bytes \n", (int)threadid, mystacksize);
   for (i=0; i<N; i++)
     for (j=0; j<N; j++)
      A[i][j] = ((i*j)/3.452) + (N-i);
   pthread_exit(NULL);
}
 
int main(int argc, char *argv[])
{
   pthread_t threads[NTHREADS];
   size_t stacksize;
   int rc, t;
 
   pthread_attr_init(&attr);
   pthread_attr_getstacksize (&attr, &stacksize);
   printf("Default stack size = %d\n", stacksize);
   stacksize = sizeof(double)*N*N+MEGEXTRA;
   printf("Amount of stack needed per thread = %d\n",stacksize);
   pthread_attr_setstacksize (&attr, stacksize);
   printf("Creating threads with stack size = %d bytes\n",stacksize);
   for(t=0; t<NTHREADS; t++){
      rc = pthread_create(&threads[t], &attr, dowork, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   printf("Created %d threads.\n", t);
   pthread_exit(NULL);
}
