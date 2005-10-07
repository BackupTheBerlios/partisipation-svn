STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/remote

compile:
	$(MAKE) -C callback
	$(MAKE) -C server

clean:
	$(MAKE) -C callback clean
	$(MAKE) -C server clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C callback style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C server style


indent:
	$(MAKE) -C callback indent
	$(MAKE) -C server indent
