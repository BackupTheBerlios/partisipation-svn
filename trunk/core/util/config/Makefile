STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/config

compile:
	$(MAKE) -C xml

clean:
	$(MAKE) -C xml clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C xml style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C xml indent
