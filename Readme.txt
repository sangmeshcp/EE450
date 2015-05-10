Name: Sangamesh patil Channabasappa Patil
Student ID: 4022324851 

What have I done in this assignment:
In this project we are simulating a distributed key pair system. There are 2 client and 3 servers. The clients only can communicate with
server 1 using UDP protocol only .. and server1 with communicate with server2 and server 2 communicate with server3 via TCP protocol. the key and 
value pair are divided among different servers.

Design :
I have Used OOP concept and create an socket class for each client and server. the idea to create classes was an idea conceived earlier for 
a product as software developer at Alcatel-lucent so that clients can reuse the code by creating a object.

Files:
dhtserver1.cpp  : it provides the UDP server for incoming clients and also a TCP client to communicate with server 2.
dhtserver2.cpp  : it provides a TCP server for communication with server1 and a TCP client to communicate with server 3.
dhtserver3.cpp  : it provides a TCP server for communication with server 2.
client1.cpp     : it provides a request to enter data for users and communicate with UDP. 
client2.cpp     : it provides a request to enter data for users and communicate with UDP.

how to compile and run the the program:

I have written a makefile to build the system 

the following command will compile all the different client and servers
make all             

to compile and run the server1 , run the following command
make server1

to compile and run the server2, run the following command 
make server2

to compile and run the server3, run the following command 
make server3 

to compile and run the client1, run the following command
make dclient1

to compile and run the client2, run the following command 
make dclient2


messages:
are as shown in the table in respective terminals .

following restrictions :
1) the compilation has to be done in following order server1, server2, server3, client1,client2
2) first one to contact server1 should be client1 and next one to contact should be client2
3) client1 should be first and client2 should be second and this has to be done alternatively for proper messages.
4) if any of the server is not running and other server tries to establish a connection it will result in seg fault error 13 .. this is 
done that way.


the whole code is inspired from earlier work of mine at Alcatel-lucent for socket framework ... I have used similar idea and done the 
code from scratch for the project.
