compile:
	find ./ | grep \\.c$$ | grep -v tests/ | xargs gcc -c -Wall -I./
	gcc -o check_osip_adapter test_osip.o osip_adapter.o -L/usr/local/lib -lcheck -leXosip2

clean:
	find ./ | grep \\.o$$ | xargs rm -f

check: compile

