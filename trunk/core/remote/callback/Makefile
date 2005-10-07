STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/callback

compile:
	$(MAKE) -C xmlrpc

clean:
	$(MAKE) -C xmlrpc clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C xmlrpc style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C xmlrpc indent
