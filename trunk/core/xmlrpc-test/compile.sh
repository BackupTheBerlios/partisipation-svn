gcc -c -I. -Wall registration_management.c 
gcc -c -I. -Wall call_management.c 
gcc -c -I. -Wall account_management.c
gcc -c -I. -Wall volume_management.c
gcc -c -I. -Wall extras_management.c
gcc -c -I. -Wall callback_util.c
gcc -c -I. -Wall gui_callback.c
gcc -c -I. -Wall xmlrpc_server.c 
gcc -o xmlrpc_server xmlrpc_server.o registration_management.o call_management.o account_management.o volume_management.o extras_management.o callback_util.o gui_callback.o -L/usr/local/lib -L/usr/lib  -L/usr/local/lib -L/usr/ssl/lib -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc_client -lpthread -lxmlrpc -lxmlparse -lxmltok -lwwwxml   -lwwwinit -lwwwapp -lwwwhtml -lwwwtelnet -lwwwnews -lwwwhttp -lwwwmime -lwwwgopher -lwwwftp -lwwwfile -lwwwdir -lwwwcache -lwwwstream -lwwwmux -lwwwtrans -lwwwcore -lwwwutils -lmd5 -ldl -lcurl -lssl -lcrypto -ldl -lz -lxmlrpc_client
