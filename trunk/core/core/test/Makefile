compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs gcc $(CFLAGS) -c -I../

	gcc $(CFLAGS) -otest_dispatcher test_dispatcher.o ../event_dispatcher.o ../../util/thread_management.o ../queue/queue.o ../queue/queue_datatype.o -lpthread
clean:
	rm test_dispatcher
