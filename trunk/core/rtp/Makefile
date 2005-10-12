INDENT_PROFILE = ../indent.pro
export INDENT_PROFILE

default:compile

compile: indent clean
	gcc -o rtp-test rtp-test.c -I/opt/gnome/include -I/opt/gnome/include/gtk-1.2 -I/opt/gnome/include/glib-1.2 -I/opt/gnome/lib/glib/include -I/usr/X11R6/include -lrtp -lm
	
clean:
	rm *~

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent