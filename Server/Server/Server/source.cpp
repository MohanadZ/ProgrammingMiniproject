#include "Server.h"

int main()
{
	Server myServer(1111); //Create server on port 1111
	for (int i = 0; i < 100; i++) //Up to 100 times...
	{
		myServer.listenForNewConnection(); //Accept new connection (if someones trying to connect)
	}
	system("pause");
	return 0;
}