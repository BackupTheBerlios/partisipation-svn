AC_INIT([PartiSIPation], [0.1-alpha], 
		[Enrico Hartung <enrico@iptel.org>], 
		[partisipation])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

USERHOME=$HOME
AC_SUBST(USERHOME)
LOCALUSER=$USER
AC_SUBST(LOCALUSER)

AC_CONFIG_FILES([Makefile
				accounts/Makefile
				config/Makefile
				config/xml/Makefile
				core/Makefile
				core/callIDs/Makefile
				core/events/Makefile
				core/gui_input/Makefile
				core/gui_output/Makefile
				core/sip_input/Makefile
				core/sip_output/Makefile
				doc/Makefile
				doc/images/Makefile
				lib/Makefile
				lib/genx/Makefile
				remote/Makefile
				remote/callback/Makefile
				remote/callback/xmlrpc/Makefile
				remote/server/Makefile
				remote/server/xmlrpc/Makefile
				rtp/Makefile
				sipstack/Makefile
				sipstack/eXosip/Makefile
				util/Makefile
				util/config/Makefile
				util/config/xml/Makefile
				util/list/Makefile
				util/logging/Makefile
				util/logging/simple/Makefile
				util/queue/Makefile
				util/threads/Makefile])
AC_OUTPUT
