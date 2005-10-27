STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/core

compile:
	$(MAKE) -C callIDs
	$(MAKE) -C events
	$(MAKE) -C gui_input
	$(MAKE) -C gui_output
	$(MAKE) -C sip_input
	$(MAKE) -C sip_output

clean:
	$(MAKE) -C callIDs clean
	$(MAKE) -C events clean
	$(MAKE) -C gui_input clean
	$(MAKE) -C gui_output clean
	$(MAKE) -C sip_input clean
	$(MAKE) -C sip_output clean

style:
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C callIDs style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C events style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C gui_input style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C gui_output style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C sip_input style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C sip_output style

indent:
	$(MAKE) -C callIDs indent
	$(MAKE) -C events indent
	$(MAKE) -C gui_input indent
	$(MAKE) -C gui_output indent
	$(MAKE) -C sip_input indent
	$(MAKE) -C sip_output indent
