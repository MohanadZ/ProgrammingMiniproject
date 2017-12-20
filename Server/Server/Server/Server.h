#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <iostream>

enum Packet
{
	P_ChatMessage,
};

class Server
{
public:
	Server(int port, bool broadcastPublically = false);
	bool listenForNewConnection();

private:
	bool sendInt(int ID, int _int);
	bool getInt(int ID, int & _int);

	bool sendPacketType(int ID, Packet _packettype);
	bool getPacketType(int ID, Packet & _packettype);

	bool sendString(int ID, std::string & _string);
	bool getString(int ID, std::string & _string);

	bool processPacket(int ID, Packet _packettype);

	static void clientHandlerThread(int ID);

private:
	SOCKET connections[100];
	int totalConnections = 0;

	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	int addrlen = sizeof(addr);
	SOCKET sListen;
};

static Server * serverptr; //Serverptr is necessary so the static ClientHandler method can access the server instance/functions.
