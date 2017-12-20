#include "Server.h"

Server::Server(int port, bool broadcastPublically) //Port = port to broadcast on. BroadcastPublically = false if server is not open to the public (people outside of your router), true = server is open to everyone (assumes that the port is properly forwarded on router settings)
{
	//Winsock Startup
	WSAData wsaData;
	WORD dllVersion = MAKEWORD(2, 1);
	if (WSAStartup(dllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		exit(1);
	}

	if (broadcastPublically) //If server is open to public
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else //If server is only for our router
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
	addr.sin_port = htons(port); //Port
	addr.sin_family = AF_INET; //IPv4 Socket

	sListen = socket(AF_INET, SOCK_STREAM, NULL); //Create socket to listen for new connections
	if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) //Bind the address to the socket, if we fail to bind the address..
	{
		std::string errorMsg = "Failed to bind the address to our listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		exit(1);
	}
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max Connections, if we fail to listen on listening socket...
	{
		std::string errorMsg = "Failed to listen on listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		exit(1);
	}
	serverptr = this;
}

bool Server::listenForNewConnection()
{
	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection
	if (newConnection == 0) //If accepting the client connection failed
	{
		std::cout << "Failed to accept the client's connection." << std::endl;
		return false;
	}
	else //If client connection properly accepted
	{
		std::cout << "Client Connected! ID: " << totalConnections << std::endl;
		connections[totalConnections] = newConnection; //Set socket in array to be the newest connection before creating the thread to handle this client's socket.
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandlerThread, (LPVOID)(totalConnections), NULL, NULL); //Create Thread to handle this client. The index in the socket array for this thread is the value (i).
		//sendString(totalConnections, std::to_string(totalConnections));
		std::string motd = "Welcome to the chat room! Your ID is: " + std::to_string(totalConnections);
		sendString(totalConnections, motd);
		totalConnections += 1; //Incremenent total # of clients that have connected
		return true;
	}
}

bool Server::processPacket(int ID, Packet _packettype)
{
	switch (_packettype)
	{
	case P_ChatMessage: //Packet Type: chat message
	{
		std::string message; //string to store our message we received
		if (!getString(ID, message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
						  //Next we need to send the message out to each user
		for (int i = 0; i < totalConnections; i++)
		{
			//if (i == ID) //If connection is the user who sent the message...
			//	continue;//Skip to the next user since there is no purpose in sending the message back to the user who sent it.

			std::string outMsg = "Client " + std::to_string(ID) + ": " + message;
			if (!sendString(i, outMsg)) //Send message to connection at index i, if message fails to be sent...
			{
				std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << i << std::endl;
			}
		}
		std::cout << "Processed chat message packet from user ID: " << ID << std::endl;
		break;
	}

	default: //If packet type is not accounted for
	{
		std::cout << "Unrecognized packet: " << _packettype << std::endl; //Display that packet was not found
		break;
	}
	}
	return true;
}

void Server::clientHandlerThread(int ID) //ID = the index in the SOCKET Connections array
{
	Packet packetType;
	while (true)
	{
		if (!serverptr->getPacketType(ID, packetType)) //Get packet type
			break; //If there is an issue getting the packet type, exit this loop
		if (!serverptr->processPacket(ID, packetType)) //Process packet (packet type)
			break; //If there is an issue processing the packet, exit this loop
	}
	std::cout << "Lost connection to client ID: " << ID << std::endl;
	closesocket(serverptr->connections[ID]);
}
