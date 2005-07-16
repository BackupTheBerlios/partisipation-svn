compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs gcc -c -I../
	gcc -I../ test.c -otest thread_management.o -lpthread
