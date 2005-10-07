STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/sipstack

compile:
	$(MAKE) -C eXosip
	
clean:
	$(MAKE) -C eXosip clean

check: compile

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C eXosip style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C eXosip indent
