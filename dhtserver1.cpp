// ************************************************
// filename: dhtserver1.cpp
//
// creator:Sangamesh Patil
// 
// Description: This is a server to contact other servers on TCP and UDP protocols to accept connection from client. 
// It is inspired by earlier implementation of UDP  and TCP classes done by myself 
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
// 15Mar2015 Sangamesh Patil   UDP socket server and TCP Client implementation
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
#include <unistd.h>
#include <errno.h>
#include <string>
#include <vector>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define TRUE 0;
#define FALSE 1;
#define UDP 21851
#define TCP 22851

using namespace std;
bool foundkey=false;
string foundvalue;
// define the hostname
const string HOSTNAME = "nunki.usc.edu";
string val="overrwrite";
int firstime=0;

// **************************************
// Class for UDP socket server
// **************************************
class udpsocketclass
{
private:
	void showErr(const char *);
	int bufferSize;
	struct sockaddr_in peer_address;
	struct sockaddr_in local_address;
public:
	int udpSocket;
	char localIP[32];
	socklen_t localPort;
	char peerIP[32];
	socklen_t peerPort;
	string recvString;
	string clientName;
	udpsocketclass(string, int, string, int);
	void closeUdpSocket();
	int start();
	int sendData(string);
	int recvData();
};

// **************************************
// function to receive data from client.
// **************************************
int udpsocketclass::recvData()
{
	ssize_t n;
	socklen_t lengthOfAddress = sizeof(struct sockaddr);
	
	// receive the length of data
	int recvLength = 0;
	if (recvfrom(this->udpSocket, &recvLength, sizeof(recvLength), 0, (struct sockaddr *)&this->peer_address, &lengthOfAddress) < 0)
	{
		this->showErr("Receive Error");
		return -1;
	}
	recvLength = ntohl(recvLength);

	// get peer IP and port
	strcpy(this->peerIP, inet_ntoa(this->peer_address.sin_addr));
	this->peerPort = ntohs(this->peer_address.sin_port);

	// receive data
	char buffer[this->bufferSize];
	memset(buffer, 0, this->bufferSize);
	if ((recvfrom(this->udpSocket, buffer, recvLength, 0, (struct sockaddr *)&this->peer_address, &lengthOfAddress)) < 0)
	{
		this->showErr("Receive Error");
		return -1;
	}
	this->recvString = buffer;

	return 1;
}

// **************************************
// function to send data to client.
// **************************************
int udpsocketclass::sendData(string str)
{
	ssize_t n;
	int sendLength = htonl(str.size());
	socklen_t lengthOfAddress = sizeof(struct sockaddr);

	// send the length of data
	if ((sendto(this->udpSocket, &sendLength, sizeof(sendLength), 0, (struct sockaddr *)&this->peer_address, lengthOfAddress)) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}

	if ((sendto(this->udpSocket, str.c_str(), str.size(), 0, (struct sockaddr *)&this->peer_address, lengthOfAddress)) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}
	return 1;
}

// **************************************
// function to start the udp server
// **************************************
int udpsocketclass::start()
{
	this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket < 0)
	{
		showErr("Socket cannot create");
	}

	if (bind(this->udpSocket, (struct sockaddr *)&this->local_address, sizeof(struct sockaddr)) < 0)
	{
		showErr("Bind Error");
		return -1;
	}

	// update port number after udp socket been created
	struct sockaddr_in udpTemp;
	socklen_t udpTempLen = sizeof(udpTemp);
	getsockname(this->udpSocket, (struct sockaddr *)&udpTemp, &udpTempLen);
	strcpy(this->localIP, inet_ntoa(udpTemp.sin_addr));
	this->localPort = ntohl(udpTemp.sin_port);

	return 0;
}

// **************************************
// function to create UDP socket server.
// **************************************
udpsocketclass::udpsocketclass(string srcHost, int srcPort, string dstHost, int dstPort)
{
	this->bufferSize = 128;
	this->localPort = srcPort;
	this->peerPort = dstPort;
	cout<<"localport"<< this->localPort<<endl;
	// get UDP local IP address
	struct hostent *local;
	local = gethostbyname(srcHost.c_str());
	strcpy(this->localIP, inet_ntoa(*((struct in_addr *)local->h_addr)));

	memset((char *) &this->local_address, 0, sizeof(this->local_address));
	this->local_address.sin_family = AF_INET;
	this->local_address.sin_port = srcPort;
	this->local_address.sin_addr = *((struct in_addr *)local->h_addr);
	cout<<this->local_address.sin_port<<endl;
	// get UDP peer IP address
	struct hostent *peer;
	peer = gethostbyname(dstHost.c_str());
	strcpy(this->peerIP, inet_ntoa(*((struct in_addr *)peer->h_addr)));

	memset((char *) &this->peer_address, 0, sizeof(this->peer_address));
	this->peer_address.sin_family = AF_INET;
	this->peer_address.sin_port = htons(dstPort);
	this->peer_address.sin_addr = *((struct in_addr *)peer->h_addr);
}

// **************************************
// function to close the UDP socket
// **************************************
void udpsocketclass::closeUdpSocket()
{
	close(this->udpSocket);
}

// **************************************
// function to show error during sockets.
// **************************************
void udpsocketclass::showErr(const char *err)
{
	perror(err);
	exit(1);
}
// **************************************
// TCP client class
// **************************************
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
	tcp_client(string, int);
	int connToServer();
	void closeClientSocket();
	int sendData(string);
	int recvData();
	string recvString;
};

// **************************************
// function to create TCP client
// **************************************
tcp_client::tcp_client(string h, int p)
{
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

// **************************************
// function to connect to server
// **************************************
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
	printf("The server 1 sends the request %s to server2.The TCP port number %d and IP address %s.\n",val.c_str(),myInfo.sin_port, inet_ntoa (myInfo.sin_addr));
	return 1;
}

// **************************************
// function to recieve data from server2
// **************************************
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
	printf("The server 1 has received the value %s from server2 with TCP port number %d and IP address %s. \n The Server1 closed the TCP connection with the Server2. \n",this->recvString.c_str(),this->portNum,inet_ntoa (server_address.sin_addr));
	return 1;
}

// **************************************
// function to send data to server2
// **************************************
int tcp_client::sendData(string str)
{
	int sendLength = htonl(str.size());

	if ((write(this->clientSocket, &sendLength, sizeof(sendLength))) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}
	int n =0;
	if ((n = write(this->clientSocket, str.c_str(), str.size())) < 0)
	{
		this->showErr("Send Error");
		return -1;
	}
	//cout << "send to buffer: " << n << endl;
	return 1;
}

// **************************************
// function to show error during sockets.
// **************************************
void tcp_client::showErr(const char *err)
{
	perror(err);
	exit(1);
}

// **************************************
// function to close the socket.
// **************************************
void tcp_client::closeClientSocket()
{
	close(this->clientSocket);
}
std ::ifstream input;
string key[20],value[20];

// **************************************
// function to read file
// **************************************
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

main()
{
	string val1;
	input.open("server1.txt");
	 fileread();
	  udpsocketclass server(HOSTNAME, UDP, HOSTNAME, INADDR_ANY);
	 printf("The Server1 has UDP port number %d and IP address %s.\n",UDP, server.localIP); 
	 while(1)
	 {
		
	 server.start();
      server.recvData();
	 val=server.recvString;	 
	 if(firstime%2==0)
		{
	 printf("The server 1 has received a request with %s from client1 with port number %d and IP address %s.\n",val.c_str(),server.peerPort,server.peerIP);
	 //cout<<"value received:"+val<<endl;
        }
		else
		{	
		 printf("The server 1 has received a request with key %s from client2 with port number %d and IP address %s.\n",val.c_str(),server.peerPort,server.peerIP);
	 //cout<<"value received:"+val<<endl;
		}
		 val1=val;
		 val1=val1.erase(0,4);
	 for(int i=0;i<20;i++)
	 {
		 //cout<<key[i];
		 if(key[i]==val1)
		 {
			foundkey=true;
			foundvalue=value[i];
		 }
     }
	 if(foundkey==true)
	 {
		//cout<<foundvalue;
		val1="POST "+foundvalue;
		if(firstime%2==0)
		{
	printf("The Server1 sends the reply %s to Client1 with port number %d and IP address %s.\n",val1.c_str(),server.peerPort,server.peerIP);
		}
		else
		{
		printf("The Server1	sends the reply %s to Client2 with port number %d and IP address %s.\n",val1.c_str(),server.peerPort,server.peerIP);	
		}
		server.sendData(val1);
	 }
	 else
	 {
		 //start tcp connection to server 2
		 tcp_client client(HOSTNAME, TCP);
	client.connToServer();
	client.sendData(val);
	client.recvData();
	foundvalue=client.recvString;
	val=foundvalue;
	server.sendData(val);
	client.closeClientSocket();
		if(firstime%2==0)
		{
	printf("The Server1 sends the reply %s to Client1 with port number %d and IP address %s. \n",val.c_str(),server.peerPort,server.peerIP);
		}
		else
		{
		printf("The Server1 sends the reply %s to Client2 with port number %d and IP address %s. \n",val.c_str(),server.peerPort,server.peerIP);	
		}
	 }
	 firstime+=1;
	 foundkey=false;
	 server.closeUdpSocket();
	 }
	input.close();
}
