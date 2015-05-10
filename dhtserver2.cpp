// ************************************************
// filename: dhtserver2.cpp
//
// creator:Sangamesh Patil
// 
// Description: If you are reading this then you have lot of time to fuck around.. stfu and read the code below asshole!!! 
//
// History
//
// date      name              description
// 09Mar2015 Sangamesh Patil   initial creation
// ***************************************************
#include <cstdlib>
#include<list>
#include<string>
#include <string.h>
#include<stdio.h>
#include <errno.h>
#include <vector>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include<iostream>
#include<sstream>
#include<stdlib.h>
#include<fstream>
#define TRUE 0;
#define TCP 220851
#define TCP1 230851
#define FALSE 1;
using namespace std;
std ::ifstream input;
string key[20],value[20];
// define the hostname
const string HOSTNAME = "nunki.usc.edu";
// class for tcp data transfer
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

// class for tcp server listen
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
class tcp_client
{
private:
	int clientSocket;
	int portNum;
	struct hostent *hostInfo;
	struct sockaddr_in server_address;
	void showErr(const char *err);
	int bufferSize;
public:
	tcp_client(string, string, int);
	int connToServer();
	void closeClientSocket();
	int sendData(string);
	int recvData();
	string recvString;
	string patientName;
};

tcp_client::tcp_client(string n, string h, int p)
{
	this->patientName = n;
	this->portNum = p;
	this->clientSocket = -1;
	this->bufferSize = 128;

	// create socket for tcp client
	this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// get information about the server
	this->hostInfo = gethostbyname(h.c_str());

	memset((char *) &this->server_address, 0, sizeof(this->server_address));

	this->server_address.sin_family = AF_INET;
	memcpy(&this->server_address.sin_addr, this->hostInfo->h_addr, this->hostInfo->h_length);
	this->server_address.sin_port = htons(this->portNum);
}

int tcp_client::connToServer()
{
	if (connect(this->clientSocket, (struct sockaddr *) &this->server_address, sizeof(this->server_address)) < 0)
	{
		return -1;
	}
	// get IP address and port number of client itself
	struct sockaddr_in myInfo;
	socklen_t myInfoLength = sizeof(myInfo);
	getsockname(this->clientSocket, (struct sockaddr*) &myInfo, &myInfoLength);

	// OUTPUT
	printf("Phase 1: %s has TCP port number %d and IP address %s.\n", this->patientName.c_str(), ntohs (myInfo.sin_port), inet_ntoa (myInfo.sin_addr));
	return 1;
}

int tcp_client::recvData()
{
	int n;
	int recvLength = 0;
	if (read(this->clientSocket, &recvLength, sizeof(recvLength)) < 0)
	{
		showErr("Read Error");
		return -1;
	}
	recvLength = ntohl(recvLength);

	char buffer[this->bufferSize];
	memset(buffer, 0, this->bufferSize);
	if ((read(this->clientSocket, buffer, recvLength) < 0))
	{
		showErr("Read Error");
		return -1;
	}
	this->recvString = buffer;
	return 1;
}

int tcp_client::sendData(string str)
{
	int sendLength = htonl(str.size());

	if ((write(this->clientSocket, &sendLength, sizeof(sendLength))) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}

	//cout << "String length is " << str.size() << endl;
	//cout << "sendLength is " << sendLength << endl;

	// char buffer[this->bufferSize];
	// memset(buffer, 0, this->bufferSize);
	// strncpy(buffer, str.c_str(), sizeof(str));
	//buffer[sizeof(buffer) - 1] = 0;

	int n =0;
	if ((n = write(this->clientSocket, str.c_str(), str.size())) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}
	//cout << "send to buffer: " << n << endl;
	return 1;
}

void tcp_client::showErr(const char *err)
{
	perror(err);
	exit(1);
}

void tcp_client::closeClientSocket()
{
	close(this->clientSocket);
}
///////////////////////////////////
/// Implement of tcp data class ///
///////////////////////////////////

tcp_data::tcp_data(int sock)
{
	this->bufferSize = 128;
	this->dataSocket = sock;
}

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
	return n;
}

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
	
	return 1;
}

void tcp_data::closeDataSocket()
{
	close(this->dataSocket);
}

void tcp_data::showErr(const char *err)
{
	perror(err);
	exit(1);
}

/////////////////////////////////////
/// Implement of tcp server class ///
/////////////////////////////////////

tcp_server::tcp_server(string s, int p)
{
	this->listenSocket = -1;
	this->hostname = s;
	this->portNum = p;
}

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
	printf("Phase 1: The Health Center Server has port number %d and IP address %s.\n", this->portNum, inet_ntoa(server_address.sin_addr));
	
	return 1;
}

void tcp_server::closeListenSocket()
{
	close(this->listenSocket);
}

int tcp_server::acceptClient()
{
	int r;
	socklen_t client_addr_length = sizeof(client_address);
	r = accept(listenSocket, (struct sockaddr *) &client_address, &client_addr_length);
	return r;
}

void tcp_server::showErr(const char *err)
{
	perror(err);
	exit(1);
}
void showErr(const char *err)
{
	perror(err);
	exit(1);
}
void fileread()
{
if (input.is_open())
 {
	 for(int i=0;i<20;i++)
	 {
	input>>key[i]>>value[i];
	 }
for(int i=0;i<20;i++)
{
cout<<key[i]<<"\t"<<value[i]<<"\n";
}	
}
}
main()
{
	cout<<"*****************yo asshole!!!! welcome to server2*****************"<<endl;
	input.open("server2.txt");
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
						cout << "receive: " << receive << endl;
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
							cout<<foundvalue;
							val=foundvalue;
							cuerrentSocket->sendData(val);
							}
							else
							{
							//start tcp connection to server 3
							tcp_client client("SERVER 1", HOSTNAME, TCP1);
							client.connToServer();
	
							// send username and password
							client.sendData(receive);
							client.recvData();
							foundvalue=client.recvString;
							cuerrentSocket->sendData(foundvalue);
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
