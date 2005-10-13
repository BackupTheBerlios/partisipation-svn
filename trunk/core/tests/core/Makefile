STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/core

compile:
	$(MAKE) -C callIDs
	$(MAKE) -C events

clean:
	$(MAKE) -C callIDs clean
	$(MAKE) -C events clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C callIDs style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C events style


indent:
	$(MAKE) -C callIDs indent
	$(MAKE) -C events indent
