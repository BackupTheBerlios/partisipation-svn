STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/tests

compile:
	$(MAKE) -C accounts
	$(MAKE) -C core
	$(MAKE) -C sipstack
	$(MAKE) -C util

clean:
	$(MAKE) -C accounts clean
	$(MAKE) -C core clean
	$(MAKE) -C sipstack clean
	$(MAKE) -C util clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C accounts style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C core style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C sipstack style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C util style


indent:
	$(MAKE) -C accounts indent
	$(MAKE) -C core indent
	$(MAKE) -C sipstack indent
	$(MAKE) -C util indent
