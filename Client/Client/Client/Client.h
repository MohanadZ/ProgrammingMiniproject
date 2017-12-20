#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib") //Required for WinSock
#include <WinSock2.h> //For win sockets
#include <string> //For std::string
#include <iostream> //For std::cout, std::endl, std::cin.getline

enum Packet
{
	P_ChatMessage,
};

class Client
{
public: //Public functions
	Client();
	Client(std::string IP, int port);
	bool connectClient();

	bool sendString(std::string & _string);
	bool closeConnection();
private: //Private functions
	bool processPacket(Packet _packettype);
	static void ClientThread();
	//Sending Funcs
	bool sendInt(int _int);
	bool sendPacketType(Packet _packettype);


	//Getting Funcs
	bool getInt(int & _int);
	bool getPacketType(Packet & _packettype);
	bool getString(std::string & _string);

private:
	SOCKET connection;//This client's connection to the server
	SOCKADDR_IN addr; //Address to be binded to our Connection socket
	int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function
};

static Client * clientptr; //This client ptr is necessary so that the ClientThread method can access the Client instance/methods. Since the ClientThread method is static, this is the simplest workaround I could think of since there will only be one instance of the client.