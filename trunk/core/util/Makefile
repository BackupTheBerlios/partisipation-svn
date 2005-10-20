STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/util

compile:
	$(MAKE) -C config
	$(MAKE) -C list
	$(MAKE) -C logging
	$(MAKE) -C queue
	$(MAKE) -C threads

clean:
	$(MAKE) -C config clean
	$(MAKE) -C list clean
	$(MAKE) -C logging clean
	$(MAKE) -C queue clean
	$(MAKE) -C threads clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C config style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C list style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C logging style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C queue style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C threads style


indent:
	$(MAKE) -C config indent
	$(MAKE) -C list indent
	$(MAKE) -C logging indent
	$(MAKE) -C queue indent
	$(MAKE) -C threads indent
