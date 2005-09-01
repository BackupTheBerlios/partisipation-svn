STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/test

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../

	$(LINK.c) -o test_dispatcher test_dispatcher.o ../event_dispatcher.o ../statemachine.o ../../util/thread_management.o ../../util/logging/logger.o ../../util/queue/queue.o ../call_id_generator.o -lpthread
	$(LINK.c) -o test_cig test_cig.o ../call_id_generator.o ../../util/thread_management.o ../../util/logging/logger.o -lcheck -lpthread

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f test_dispatcher
	rm -f test_cig

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
