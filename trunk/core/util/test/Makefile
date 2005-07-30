STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/test

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../

	$(LINK.c) -o test_thread_management test_thread_management.o ../thread_management.o -lpthread

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f test_thread_management

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
