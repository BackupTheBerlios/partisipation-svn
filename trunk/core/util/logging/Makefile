STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/logger

compile:
	$(MAKE) -C simple

clean:
	$(MAKE) -C simple clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C simple style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C simple indent
