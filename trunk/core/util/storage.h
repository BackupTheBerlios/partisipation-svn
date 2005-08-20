
/** 
 * @file storage.h
 * Type definitions for thread management.
 *
 * Because only a pointer can be handed over to the start routine of a thread,
 * a struct is needed to pack the needed arguments.
 *
 * @author Matthias Liebig
 */
#ifndef HSTORAGE_USED
#define HSTORAGE_USED

/**
 * This struct is needed to pass the arguments of start_thread() to 
 * add_thread() which is a thread itself.
 */
typedef struct {

   /**
    * The initial function that will be called when the thread is started.
    */
	void *(*start_routine) (void *);

   /**
    * The arguments of the start routine - a type-free pointer
    */
	void *args;
} thread_data;

#endif
