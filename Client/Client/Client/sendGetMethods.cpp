#include "Client.h"

bool Client::sendInt(int _int)
{
	int retnCheck = send(connection, (char*)&_int, sizeof(int), NULL); //send int: _int
	if (retnCheck == SOCKET_ERROR) //If int failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

bool Client::getInt(int & _int)
{
	int retnCheck = recv(connection, (char*)&_int, sizeof(int), NULL); //receive integer
	if (retnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not get the integer
	return true;//Return true if we were successful in retrieving the int
}

bool Client::sendPacketType(Packet _packettype)
{
	int retnCheck = send(connection, (char*)&_packettype, sizeof(Packet), NULL); //Send packet: _packettype
	if (retnCheck == SOCKET_ERROR) //If packettype failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

bool Client::getPacketType(Packet & _packettype)
{
	int retnCheck = recv(connection, (char*)&_packettype, sizeof(Packet), NULL); //receive packet type (same as integer)
	if (retnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not properly get the packet type
	return true;//Return true if we were successful in retrieving the packet type
}

bool Client::sendString(std::string & _string)
{
	if (!sendPacketType(P_ChatMessage)) //Send packet type: Chat Message, If sending packet type fails...
		return false; //Return false: Failed to send string
	int bufferlength = _string.size(); //Find string buffer length
	if (!sendInt(bufferlength)) //Send length of string buffer, If sending buffer length fails...
		return false; //Return false: Failed to send string buffer length
	int retnCheck = send(connection, _string.c_str(), bufferlength, NULL); //Send string buffer
	if (retnCheck == SOCKET_ERROR) //If failed to send string buffer
		return false; //Return false: Failed to send string buffer
	return true; //Return true: string successfully sent
}

bool Client::getString(std::string & _string)
{
	int bufferlength; //Holds length of the message
	if (!getInt(bufferlength)) //Get length of buffer and store it in variable: bufferlength
		return false; //If get int fails, return false
	char * buffer = new char[bufferlength + 1]; //Allocate buffer
	buffer[bufferlength] = '\0'; //Set last character of buffer to be a null terminator so we aren't printing memory that we shouldn't be looking at
	int retnCheck = recv(connection, buffer, bufferlength, NULL); //receive message and store the message in buffer array, set RetnCheck to be the value recv returns to see if there is an issue with the connection
	_string = buffer; //set string to received buffer message
	delete[] buffer; //Deallocate buffer memory (cleanup to prevent memory leak)
	if (retnCheck == SOCKET_ERROR) //If connection is lost while getting message
		return false; //If there is an issue with connection, return false
	return true;//Return true if we were successful in retrieving the string
}