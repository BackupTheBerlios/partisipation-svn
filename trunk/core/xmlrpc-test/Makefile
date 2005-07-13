GCC_FLAGS = -L/usr/local/lib -L/usr/lib  -L/usr/local/lib -L/usr/ssl/lib -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc_client -lpthread -lxmlrpc -lxmlparse -lxmltok -lwwwxml   -lwwwinit -lwwwapp -lwwwhtml -lwwwtelnet -lwwwnews -lwwwhttp -lwwwmime -lwwwgopher -lwwwftp -lwwwfile -lwwwdir -lwwwcache -lwwwstream -lwwwmux -lwwwtrans -lwwwcore -lwwwutils -lmd5 -ldl -lcurl -lssl -lcrypto -ldl -lz -lxmlrpc_client
BINARY_FILE = xmlrpc_server

compile:
	find ./ | grep \\.c$$ | grep -v docs/ | xargs gcc -c -I ./
	find ./ | grep \\.o$$ | grep -v docs/ | xargs gcc -o ${BINARY_FILE} $(GCC_FLAGS)

clean:
	rm -f *.o */*.o */*/*.o