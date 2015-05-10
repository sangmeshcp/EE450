############################################################
#
# non-recursive makefile for creating agent and run target based system
#
# 2015/march/08 - Initial Creation - sangamesh patil
############################################################

all:	
	g++ -g dhtserver1.cpp -o dhtserver1 -lsocket -lnsl
	g++ -g dhtserver2.cpp -o dhtserver2 -lsocket -lnsl
	g++ -g dhtserver3.cpp -o dhtserver3 -lsocket -lnsl
	g++ -g client1.cpp -o 1client  -lsocket -lnsl
	g++ -g client2.cpp -o 2client    -lsocket -lnsl
	

server1:	
	g++ -g dhtserver1.cpp -o dhtserver1 -lsocket -lnsl
	./dhtserver1

server2:	
	g++ -g dhtserver2.cpp -o dhtserver2 -lsocket -lnsl
	./dhtserver2

server3:	
	g++ -g dhtserver3.cpp -o dhtserver3 -lsocket -lnsl
	./dhtserver3

dclient1:
	g++ -g client1.cpp -o 1client  -lsocket -lnsl
	./1client

dclient2:
	g++ -g client2.cpp -o 2client  -lsocket -lnsl
	./2client

clean:	/usr/bin/rm dhtserver1
	/usr/bin/rm dhtserver2
	/usr/bin/rm dhtserver3
	/usr/bin/rm 1client
	/usr/bin/rm 2client
	




