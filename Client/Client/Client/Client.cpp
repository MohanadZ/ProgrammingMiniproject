#include "Client.h"

Client::Client(){}

bool Client::processPacket(Packet _packettype)
{
	switch (_packettype)
	{
	case P_ChatMessage: //If packet is a chat message packet
	{
		std::string message; //string to store our message we received
		if (!getString(message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false

		std::string outMsg = message + "\n";
		OutputDebugStringA(outMsg.c_str()); //Display the message to the user
		break;
	}
	default: //If packet type is not accounted for
		std::cout << "Unrecognized packet: " << _packettype << std::endl; //Display that packet was not found
		break;
	}
	return true;
}

void Client::ClientThread()
{
	Packet packetType;
	while (true)
	{
		if (!clientptr->getPacketType(packetType)) //Get packet type
			break; //If there is an issue getting the packet type, exit this loop
		if (!clientptr->processPacket(packetType)) //Process packet (packet type)
			break; //If there is an issue processing the packet, exit this loop
	}
	std::cout << "Lost connection to the server." << std::endl;
	if (clientptr->closeConnection()) //Try to close socket connection..., If connection socket was closed properly
	{
		std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else //If connection socket was not closed properly for some reason from our function
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}
}

Client::Client(std::string IP, int port)
{
	//Winsock Startup
	WSAData wsaData;
	WORD dllVersion = MAKEWORD(2, 1);
	if (WSAStartup(dllVersion, &wsaData) != 0)
	{
		std::cout << "Winsock startup failed" << std::endl;
		exit(0);
	}

	addr.sin_addr.s_addr = inet_addr(IP.c_str()); //IP address the client is looking for the server 
	addr.sin_port = htons(port); //Port to connect on
	addr.sin_family = AF_INET; //IPv4 Socket
	clientptr = this; //Update ptr to the client which will be used by our client thread
}

bool Client::connectClient()
{
	connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	if (connect(connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //If we are unable to connect...
	{
		std::cout << "Unable to connect to client" << std::endl;
		return false;
	}

	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); //Create the client thread that will receive any data that the server sends.
	return true;
}

bool Client::closeConnection()
{
	if (closesocket(connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK) //If socket error is that operation is not performed on a socket (This happens when the socket has already been closed)
			return true; //return true since connection has already been closed
		std::string errorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";

		std::cout << errorMessage << std::endl;
		return false;
	}
	return true;
}