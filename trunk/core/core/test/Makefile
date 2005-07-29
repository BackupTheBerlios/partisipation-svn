STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/test

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../

	$(LINK.c) -o test_dispatcher test_dispatcher.o ../event_dispatcher.o ../../util/thread_management.o ../queue/queue.o ../queue/queue_datatype.o -lpthread

clean:
	find ./ | grep \\.o$$ | xargs rm -f
	rm -f test_dispatcher

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c
