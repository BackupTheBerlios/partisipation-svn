compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs gcc -c -I ./

	$(MAKE) -C queue
	$(MAKE) -C test

clean:
	rm -f *.o */*.o */*/*.o
	$(MAKE) -C test clean
