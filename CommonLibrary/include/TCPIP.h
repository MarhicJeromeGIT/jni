#ifndef TCPIP_H
#define TCPIP_H

#ifndef __ANDROID__

#ifdef WIN32 /* si vous êtes sous Windows */

#include <winsock2.h>

#elif defined (__linux__) /* si vous êtes sous Linux */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else /* sinon vous êtes sur une plateforme non supportée */

#error not defined for this platform



#endif
#include <string.h>
#include <string>
#include "assert.h"

class ITweakable;

#define ETX 3

enum COMMANDE
{
	UNKNOWN_COMMAND,
	SEND_ME_TWEAK_LIST,
	TWEAK_MODIFIED, // client send server a modified tweak
	ALL_TWEAK_LIST, // server send all the tweak list

	MAX_COMMANDE,
};

std::string serializeCommande( COMMANDE cmd );
COMMANDE deserializeCommande( std::string s );

//*************************
// Serveur
//
//*************************

class Serveur
{
public:
	// the listener socket
	SOCKET sock;

	static const int maxConnection = 5;
	int currentConnection;
	SOCKET csock[maxConnection];
	int nbConnection;
public:
	Serveur();

	void Initialize();
	void Deinitialize();

	//a thread that waits to receive from a particular socket :
	void SocketReceiverThread( const SOCKET csock, const SOCKADDR_IN& csin );

	void SendModifiedTweak(ITweakable* tweak);
	~Serveur();

};

//*************************
// Client
//
//*************************

class Client
{
public:
	SOCKET sock;

public:
	Client();

	void ConnectToServer();
	void Deinitialize();

	void sendToServer( const char* buffer );
	void receiveFromServer( char* buffer, int size );

	~Client();

};

#endif




#endif // ANDROID