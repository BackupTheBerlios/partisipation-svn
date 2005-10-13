XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/callIDs

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../../../
	$(LINK.c) -o test_cig test_cig.o ../../../core/callIDs/call_id_generator.o ../../../util/threads/thread_management.o ../../../util/logging/simple/logger.o ../../../util/config/xml/config_reader.o -lcheck -lpthread $(LIBS_CLIENT) $(LIBS_ABYSS)

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f test_cig

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
