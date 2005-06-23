gcc -I. -c thread_management.c
gcc -I. test.c -otest thread_management.o -lpthread
