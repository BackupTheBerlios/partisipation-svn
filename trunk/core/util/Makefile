compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs gcc $(CFLAGS) -c -I./

	$(MAKE) -C test

clean:
	rm -f *.o */*.o */*/*.o
	$(MAKE) -C test clean
