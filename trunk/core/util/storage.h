#ifndef HSTORAGE_USED
#define HSTORAGE_USED

typedef struct 
 {
   void*(*start_routine) (void *);
   void* args;
 } thread_data;
 
#endif
