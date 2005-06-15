cd registration
gcc -c -I.. -Wall registration_management.c 
cd xmlrpc
gcc -c -I../.. -Wall dispatcher.c
cd ../../calls
gcc -c -I.. -Wall call_management.c 
cd xmlrpc
gcc -c -I../.. -Wall dispatcher.c
cd ../../accounts
gcc -c -I.. -Wall account_management.c
cd xmlrpc
gcc -c -I../.. -Wall dispatcher.c
cd ../../volume
gcc -c -I.. -Wall volume_management.c
cd xmlrpc
gcc -c -I../.. -Wall dispatcher.c
cd ../../extras
gcc -c -I.. -Wall extras_management.c
cd xmlrpc
gcc -c -I../.. -Wall dispatcher.c
cd ../../callback
gcc -c -I.. -Wall callback_util.c
gcc -c -I.. -Wall gui_callback.c
cd ..
gcc -c -I. -Wall xmlrpc_server.c 
gcc -o xmlrpc_server xmlrpc_server.o registration/registration_management.o registration/xmlrpc/dispatcher.o calls/call_management.o calls/xmlrpc/dispatcher.o accounts/account_management.o accounts/xmlrpc/dispatcher.o volume/volume_management.o volume/xmlrpc/dispatcher.o extras/extras_management.o extras/xmlrpc/dispatcher.o callback/callback_util.o callback/gui_callback.o -L/usr/local/lib -L/usr/lib  -L/usr/local/lib -L/usr/ssl/lib -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc_client -lpthread -lxmlrpc -lxmlparse -lxmltok -lwwwxml   -lwwwinit -lwwwapp -lwwwhtml -lwwwtelnet -lwwwnews -lwwwhttp -lwwwmime -lwwwgopher -lwwwftp -lwwwfile -lwwwdir -lwwwcache -lwwwstream -lwwwmux -lwwwtrans -lwwwcore -lwwwutils -lmd5 -ldl -lcurl -lssl -lcrypto -ldl -lz -lxmlrpc_client
