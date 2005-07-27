compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs gcc $(CFLAGS) -c -I../

	gcc $(CFLAGS) -otest_thread_management test_thread_management.o ../thread_management.o -lpthread

clean:
	rm test_thread_management
