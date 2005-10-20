XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/threads

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../../../

	$(LINK.c) -o test_thread_management test_thread_management.o ../../../util/threads/thread_management.o ../../../util/logging/simple/logger.o ../../../util/list/list.o ../../../util/config/xml/config_reader.o -lpthread -lcheck $(LIBS_CLIENT) $(LIBS_ABYSS)

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f test_thread_management

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
