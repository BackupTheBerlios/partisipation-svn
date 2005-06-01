gcc -c -I. -Wall registration_management.c 
gcc -c -I. -Wall call_management.c 
gcc -c -I. -Wall xmlrpc_server.c 
gcc -o xmlrpc_server xmlrpc_server.o registration_management.o call_management.o -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lpthread -lxmlrpc -lxmlparse -lxmltok
