// ************************************************
// filename: dhtserver3.cpp
//
// creator:Sangamesh Patil
// 
// Description: This is a server to contact other servers on TCP protocol and accept request from client. 
// It is inspired by earlier implementation of TCP classes done by myself as software engineer
//
// This computer program is protected.
// Recipient is to retain the program in confidence, and
// is not permitted to copy, use, distribute, modify or
// translate the program without authorization from me.
//
// History
//
// date      name              description
// 09Mar2015 Sangamesh Patil   initial creation and file reading 
// 25Mar2015 Sangamesh Patil   socket server and TCP Client implementation
// 17Apr2015 Sangamesh Patil   Final Fix
// ***************************************************

#include <cstdlib>
#include<list>
#include<string>
#include <string.h>
#include<stdio.h>
#include<iostream>
#include<sstream>
#include<stdlib.h>
#include<fstream>
#include <errno.h>
#include <netdb.h>
#include <vector>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define TRUE 0;
#define TCP 23851
#define FALSE 1;
using namespace std;

std ::ifstream input;
string key[20],value[20];
// define the hostname
const string HOSTNAME = "nunki.usc.edu";

// *********************************
//class for tcp data transfer
// *********************************
class tcp_data
{
private:
	int bufferSize;
public:
	int dataSocket;
	string recvString;
	tcp_data(int);
	int recvData();
	int sendData(string);
	void closeDataSocket();
	void showErr(const char *);
	char peerSocketIP[32];
	int peerSocketPort;
	char peerUserName[32];
};
// *********************************
// function to show error 
// *********************************
void showErr(const char *err)
{
	perror(err);
	exit(1);
}
// *********************************
// class for tcp server listen
// *********************************
class tcp_server
{
private:
	string hostname;
	int portNum;
	struct sockaddr_in server_address, client_address;
public:
	int listenSocket;
	tcp_server(string, int);
	void closeListenSocket();
	int acceptClient();
	int startListen();
	void showErr(const char *);
};
// *********************************
// Implement of tcp data class 
// *********************************
// *********************************
//function to set buffer size
// *********************************
tcp_data::tcp_data(int sock)
{
	this->bufferSize = 128;
	this->dataSocket = sock;
}
// *********************************
// function to recieve data from server2.
// *********************************
int tcp_data::recvData()
{
	int n;
	int recvLength = 0;
	n = read(this->dataSocket, &recvLength, sizeof(recvLength));
	recvLength = ntohl(recvLength);

	//cout << "recvLength is " << recvLength << endl;

	char buffer[this->bufferSize];
	memset(buffer, 0, this->bufferSize);
	n = read(this->dataSocket, buffer, recvLength);
	//cout << "receive length from buffer: " << n << endl;
	recvString = buffer;
	printf("The Server3 has received a request with %s from the Server2 with port number %d and IP address %s. \n",recvString.c_str(),this->peerSocketPort,this->peerSocketIP);
	return n;
}

// *********************************
// function to send data to server2.
// *********************************
int tcp_data::sendData(string str)
{
	int sendLength = htonl(str.size());


	if ((write(this->dataSocket, &sendLength, sizeof(sendLength))) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}

	char buffer[this->bufferSize];
	memset(buffer, 0, this->bufferSize);
	strncpy(buffer, str.c_str(), sizeof(str));
	buffer[sizeof(buffer) - 1] = 0;
	
	if ((write(this->dataSocket, str.c_str(), str.size())) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}
	printf("The Server3 sends the reply %s to Server2 with port number %d and IP address %s. \n",str.c_str(),this->peerSocketPort,this->peerSocketIP);
	return 1;
}


// *********************************
// function to close the socket
// *********************************
void tcp_data::closeDataSocket()
{
	close(this->dataSocket);
}


// *********************************
// function to show error
// *********************************
void tcp_data::showErr(const char *err)
{
	perror(err);
	exit(1);
}

// **********************************
// Implement of tcp server class 
// **********************************



// *********************************
// function to create server
// *********************************
tcp_server::tcp_server(string s, int p)
{
	this->listenSocket = -1;
	this->hostname = s;
	this->portNum = p;
}

// *********************************
// function to start listening 
// *********************************
int tcp_server::startListen()
{
	struct hostent *hostInfo;
	hostInfo = gethostbyname(this->hostname.c_str());

	this->listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	memset((char*) &server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	memcpy(&server_address.sin_addr, hostInfo->h_addr, hostInfo->h_length);
	//server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(this->portNum);
	if (bind(this->listenSocket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	{
		this->showErr("Bind Error");
		return -1;
	}
	
	if (listen(this->listenSocket, 5) < 0)
	{
		this->showErr("Listen Error");
		return -1;
	}

	// OUTPUT
	printf("The Server3 has TCP port number has port number %d and IP address %s.\n", this->portNum, inet_ntoa(server_address.sin_addr));
	
	return 1;
}

// *********************************
// function to close the socket.
// *********************************
void tcp_server::closeListenSocket()
{
	close(this->listenSocket);
}


// *********************************
// function to accept client 
// *********************************
int tcp_server::acceptClient()
{
	int r;
	socklen_t client_addr_length = sizeof(client_address);
	r = accept(listenSocket, (struct sockaddr *) &client_address, &client_addr_length);
	return r;
}


// *********************************
// function to show error.
// *********************************
void tcp_server::showErr(const char *err)
{
	perror(err);
	exit(1);
}

// ***********************************
// read the file and store the values in the string
// *************************************
void fileread()
{
if (input.is_open())
 {
	 for(int i=0;i<20;i++)
	 {
	input>>key[i]>>value[i];
	 }	
}
}

// *********************************
// main
// *********************************
main()
{
	input.open("server3.txt");
	 fileread();
	 tcp_server server(HOSTNAME, TCP);
	server.startListen();
	fd_set all_fd;
	fd_set read_fd;
	int fdmax;

	FD_ZERO(&all_fd);
	FD_ZERO(&read_fd);

	// add listen socket into all_fd
	FD_SET(server.listenSocket, &all_fd);
	fdmax = server.listenSocket;

	int socketReady;

	int socketAccept;
	int recvResult;
	string receive;
	string foundvalue,val;
	string indexChoice;

	// save all the data socket
	vector<tcp_data> savedDataSocket;
	vector<tcp_data>::iterator cuerrentSocket;

	while(1)
	{
		bool foundkey=false;
		// make read_fd as all_fd then select active socket
		read_fd = all_fd;
		socketReady = select(fdmax+1, &read_fd, NULL, NULL, NULL);

		if (socketReady == -1)
		{
			showErr("select error");
		}

		// go through all sockets less than fdmax+1
		for (int i = 0; i <= fdmax; ++i)
		{
			// find a active socket
			if (FD_ISSET(i, &read_fd))
			{
				// accept a new connection if the listen socket active
				if (i == server.listenSocket)
				{
					socketAccept = server.acceptClient();

					if (socketAccept > fdmax)
					{
						fdmax = socketAccept;
					}

					// add new connection socket to all_fd
					FD_SET(socketAccept, &all_fd);


					// add new socket to vector
					tcp_data temp(socketAccept);

					// get the peer socket IP and port
					struct sockaddr_in peerInfo;
					socklen_t peerInfoLength = sizeof(peerInfo);
					getpeername(socketAccept, (struct sockaddr*) &peerInfo, &peerInfoLength);
					strcpy(temp.peerSocketIP, inet_ntoa(peerInfo.sin_addr));
					temp.peerSocketPort = ntohs(peerInfo.sin_port);

					savedDataSocket.push_back(temp);
				}
				else
				{
					// find the saved socket, save the iterator to the savedDataSocket
					for (cuerrentSocket = savedDataSocket.begin(); cuerrentSocket != savedDataSocket.end(); cuerrentSocket++)
					{
						if (i == cuerrentSocket->dataSocket)
						{
							break;
						}
					}

					int recvResult = cuerrentSocket->recvData();

					// the client close the connection
					if (recvResult <= 0)
					{
						// peer close the connection
						cuerrentSocket->closeDataSocket();
						FD_CLR(i, &all_fd);

						// delete the socket from the vector
						savedDataSocket.erase(cuerrentSocket);
					}
					else
					{
						// get the key that client sent
						receive = cuerrentSocket->recvString;
						receive=receive.erase(0,4);
						for(int i=0;i<20;i++)
							{
								//cout<<key[i];
								if(key[i]==receive)
								{
								foundkey=true;
								foundvalue=value[i];
								}
							}
							if(foundkey==true)
							{
							//cout<<foundvalue;
							val="POST "+foundvalue;
							cuerrentSocket->sendData(val);
							}
						
						}
					}
				}
			}
		}
	
	server.closeListenSocket();
	input.close();
	return 0;
}
