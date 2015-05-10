// ************************************************
// filename: client1.cpp
//
// creator:Sangamesh Patil
// 
// Description: This is a client to contact server. It works on UDP protocol and code is inspired by earlier 
// implementation of UDP classes done by myself as software engineer 
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
// 15Mar2015 Sangamesh Patil   added UDP client 
// 17Apr2015 Sangamesh Patil   final fix
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
using namespace std;
std ::ifstream input;
string key[20],value[20];
string inputkey;
bool foundkey=false;
string foundvalue;
const string HOSTNAME = "nunki.usc.edu";

// **************************************
// Class for UDP socket client
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
// function to receive date from server.
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
	this->peerPort = ntohl(this->peer_address.sin_port);

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
// function to send date to server.
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
// function to start the udp client
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
	this->localPort = ntohs(udpTemp.sin_port);

	return 0;
}

// **************************************
// function to create UDP socket client.
// **************************************
udpsocketclass::udpsocketclass(string srcHost, int srcPort, string dstHost, int dstPort)
{
	this->bufferSize = 128;
	this->localPort = srcPort;
	this->peerPort = dstPort;

	// get UDP local IP address
	struct hostent *local;
	local = gethostbyname(srcHost.c_str());
	strcpy(this->localIP, inet_ntoa(*((struct in_addr *)local->h_addr)));

	memset((char *) &this->local_address, 0, sizeof(this->local_address));
	this->local_address.sin_family = AF_INET;
	this->local_address.sin_port = htons(srcPort);
	this->local_address.sin_addr = *((struct in_addr *)local->h_addr);

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
// function to read the file 
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
// **************************************
// Main 
// **************************************
main()
{
	string val="overrwrite";
	 input.open("client1.txt");
	 fileread();
	input.close();
	cout<<"Please Enter Your Search (USC, UCLA etc.):"<<endl;
	cin>>inputkey;
	for(int i=0;i<20;i++)
	{
		//cout<<key[i]<<endl;
		if(key[i]==inputkey)
		{
			foundkey=true;
			foundvalue=value[i];
		}
	}
	if(foundkey==true)
	{
	cout<<"The Client1 has received a request with search word "+inputkey+", which maps to "+ foundvalue+"." <<endl;
	udpsocketclass udpClient(HOSTNAME, INADDR_ANY, HOSTNAME,UDP);
	udpClient.start();
	string getvalue="GET " +foundvalue;
	printf("The Client1 sends the request %s to the Server 1 with port number %d and IP address %s.\n",getvalue.c_str(),udpClient.peerPort,udpClient.localIP);
	printf("The Client1 port number is %d and IP address is %s.\n",udpClient.localPort,udpClient.localIP);
	udpClient.sendData(getvalue);
	udpClient.recvData();
	val=udpClient.recvString;
	printf("The Client1 received the value %s from the Server 1 with port number %d and IP address %s.\n",val.c_str(),ntohl(udpClient.peerPort),udpClient.localIP);
	printf("The Client1 port number is %d and IP address is %s.\n",udpClient.localPort,udpClient.localIP);
	udpClient.closeUdpSocket();
	}
	else
	{
	cout<<"The Client1 has received a request with search word"+inputkey+", which doesn't maps to any key..bye!!!"<<endl;	
	}	
}
