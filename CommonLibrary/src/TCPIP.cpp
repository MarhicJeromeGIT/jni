#include "TCPIP.h"

// http://broux.developpez.com/articles/c/sockets/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>

#include "Tweaker.h"

#define PORT 10016
#define BUFFER_SIZE 2048

//****************************//
// Helper function
//
//****************************//


#ifndef __ANDROID__

using std::string;
using std::cout;
using std::endl;

static std::string SERIALIZE_COMMANDE[MAX_COMMANDE]=
{
	"NAN",	// UNKNOWN_COMMAND
	"STL",  // SEND_ME_TWEAK_LIST
	"TWM",	// TWEAK_MODIFIED
	"ATL",  // ALL_TWEAK_LIST
};

string serializeCommande( COMMANDE cmd)
{
	return SERIALIZE_COMMANDE[cmd];
}

COMMANDE deserializeCommande( string s )
{
	COMMANDE cmd = UNKNOWN_COMMAND;
	for( int i=0; i< MAX_COMMANDE; i++ )
	{
		if( s == SERIALIZE_COMMANDE[i] )
		{
			cmd = static_cast<COMMANDE>(i);
			break;
		}
	}
	return cmd;
}

//****************************//
// Fonctions a appeler quand on utiliser windows
// a l'init et deinit time
//
//****************************//

static void init(void)
{
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

static void end(void)
{
#ifdef WIN32
    WSACleanup();
#endif
}

//*************************
// Serveur
//
//*************************

Serveur::Serveur()
{
	init();
	sock = INVALID_SOCKET;
	currentConnection = 0;
	nbConnection = 0;
	for( int i=0; i< maxConnection; i++ )
	{
		csock[i] = INVALID_SOCKET;
	}
}

Serveur::~Serveur()
{
	cout<<"closing sockets"<<endl;
	end();

	closesocket(sock);
	for( int i=0; i< maxConnection; i++ )
	{
		closesocket(csock[i]);
	}
}

void Serveur::Initialize()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
	    perror("socket()");
	    exit(-1);
	}

	SOCKADDR_IN sin = { 0 };

	sin.sin_addr.s_addr = htonl(INADDR_ANY); /* nous sommes un serveur, nous acceptons n'importe quelle adresse */

	sin.sin_family = AF_INET;

	sin.sin_port = htons(PORT);

	// lose the pesky "Address already in use" error message
	int yes = 0;
#ifdef __linux__
	if(setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR , &yes, sizeof(int)) == -1)
	{
		perror("setsockopt()");
		exit(-1);
	}
#endif

	if( ::bind (sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
	{
	    perror("bind()");
	    exit(-1);
	}

	std::thread threads[maxConnection];

	while( 1 ) // connexion loop
	{
		cout<<"waiting for connection"<<endl;

		if(listen(sock, maxConnection) == SOCKET_ERROR)
		{
			perror("listen()");
			exit(-1);
		}

		// the client socket
		SOCKADDR_IN csin = { 0 };

		int sinsize = sizeof( csin );
		while( (csock[nbConnection] = accept(sock, (SOCKADDR *)&csin, &sinsize)) == INVALID_SOCKET )
		{
			perror("accept()");
		}

		cout<<"client connected !"<<endl;

		threads[nbConnection] = std::thread( [this,csin]()
		{
			this->SocketReceiverThread(csock[nbConnection],csin);
		});
		currentConnection = nbConnection;
		nbConnection++;
		if( nbConnection == maxConnection )
			nbConnection = 0;
	}
}

void Serveur::SocketReceiverThread( const SOCKET csock, const SOCKADDR_IN& csin )
{
	cout<<"listening to client in socket "<<csock<<endl;
	while(1)
	{
		cout<<"listening..."<<endl;
	   fd_set readfs;
	   FD_ZERO(&readfs);
	   FD_SET(csock, &readfs);

	   int ret = 0;
	   if((ret = select(csock + 1, &readfs, NULL, NULL, NULL)) < 0)
	   {
	      perror("select()");
	      exit(errno);
	   }

	   if(FD_ISSET(csock, &readfs))
	   {
	      /// des données sont disponibles sur le socket 
	      /// traitement des données 

		    cout<<"received data"<<endl;

			// receive the tweaks :
			string xmlcontent;

			bool etx = false;
			while( !etx )
			{
				char buf[2048];
				int n = 0;
				if((n = recv(csock, buf, 2048 - 1, 0)) < 0)
				{
					perror("recv()");
					exit(-1);
				}

				if( n == 0 )
				{
					cout<<"connexion closed by client"<<endl;
					return;
				}

				xmlcontent += buf;

				if( buf[ n-1 ] == ETX )
				{
					etx = true;
				}
			}

			TiXmlDocument doc;
			doc.Parse(xmlcontent.c_str(), 0, TIXML_ENCODING_UTF8);
			TiXmlHandle hdl(&doc);

			COMMANDE commande = UNKNOWN_COMMAND;
			{
				TiXmlElement* CommandeElement = hdl.FirstChildElement("commande").Element();
				assert( CommandeElement != NULL && CommandeElement->Attribute("type") != NULL);
				commande = deserializeCommande( CommandeElement->Attribute("type") );
				cout<<"commande = "<<commande<<endl;
			}

			if( commande == SEND_ME_TWEAK_LIST )
			{
				TiXmlDocument newdoc;

			    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "utf-8", "");
			    newdoc.LinkEndChild( decl );

			    TiXmlElement* CommandeElement = new TiXmlElement( "commande" );
			    CommandeElement->SetAttribute("type", serializeCommande(ALL_TWEAK_LIST) );
			    newdoc.LinkEndChild( CommandeElement );

			    TiXmlElement* Tweaks = new TiXmlElement( "tweaks" );
			    newdoc.LinkEndChild( Tweaks );


				for( int i=0; i<Tweaker::get()->tweaks.size(); i++ )
				{
					TiXmlElement* TweakElement = new TiXmlElement( "tweak" );
					Tweaks->LinkEndChild( TweakElement );

					Tweaker::get()->tweaks[i]->serialize(TweakElement);
				}

				TiXmlPrinter printer;
				printer.SetIndent( "    " );

				newdoc.Accept( &printer );
				std::string xmltext = printer.CStr();

				xmltext += ETX;
				cout<<xmltext<<endl;

				if(send(csock, xmltext.c_str(), strlen(xmltext.c_str()), 0) < 0)
				{
					perror("send()");
					exit(-1);
				}
			}

			if( commande == TWEAK_MODIFIED )
			{
				cout<<"TWEAK_MODIFIED..."<<endl;
				TiXmlElement* Tweaks = hdl.FirstChildElement("tweaks").Element();
				assert( Tweaks != NULL );
				TiXmlElement* TweakElement = Tweaks->FirstChildElement("tweak");
				assert( TweakElement != NULL );

				ITweakable* tweak = ITweakable::deserialize( TweakElement );
				assert( tweak != NULL );

				// ok look for the same tweak in our tweak list :
				for( int i=0; i< Tweaker::get()->tweaks.size(); i++ )
				{
					if( (Tweaker::get()->tweaks[i]->TWEAK_ID) == (tweak->TWEAK_ID) )
					{
						cout<<"CALLBACK"<<endl;
						Tweaker::get()->tweaks[i]->copyValue( tweak );
						Tweaker::get()->tweaks[i]->do_callback();
						break;
					}
					else
					{
						cout<<"Tweak ID = "<<Tweaker::get()->tweaks[i]->TWEAK_ID<<" != "<<tweak->TWEAK_ID<<endl;
					}
				}

			}
	   }
	}
}

void Serveur::SendModifiedTweak(ITweakable* tweak)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "utf-8", "");
	doc.LinkEndChild( decl );

	TiXmlElement* Commande = new TiXmlElement( "commande" );
	doc.LinkEndChild( Commande );
	Commande->SetAttribute("type", serializeCommande(TWEAK_MODIFIED).c_str() );

	TiXmlElement* Tweaks = new TiXmlElement( "tweaks" );
	doc.LinkEndChild( Tweaks );
	TiXmlElement* TweakElement = new TiXmlElement( "tweak" );
	Tweaks->LinkEndChild( TweakElement );
	tweak->serialize(TweakElement);

	TiXmlPrinter printer;
	printer.SetIndent( "    " );

	doc.Accept( &printer );
	std::string xmltext = printer.CStr();
	xmltext += ETX;

	if(send(csock[currentConnection], xmltext.c_str(), strlen(xmltext.c_str()), 0) < 0)
	{
		perror("send()");
		//exit(-1);
	}

}

void Serveur::Deinitialize()
{
	closesocket(sock);
}


//*************************
// Client
//
//*************************


Client::Client()
{
	init();


	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
	    perror("socket()");
	    //exit(-1);
	}
}

void Client::ConnectToServer()
{
	struct hostent *hostinfo = NULL;
	SOCKADDR_IN sin = { 0 }; /* initialise la structure avec des 0 */
	const char *hostname = "127.0.0.1";

	hostinfo = gethostbyname(hostname); /* on récupère les informations de l'hôte auquel on veut se connecter */
	if (hostinfo == NULL) /* l'hôte n'existe pas */
	{
	    printf( "Unknown host %s.\n", hostname);
	    exit(EXIT_FAILURE);
	}

	sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port = htons(PORT); /* on utilise htons pour le port */
	sin.sin_family = AF_INET;

	if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
	    perror("connect()");
	    //exit(-1);
	}

	cout<<"connected to server"<<endl;
}

void Client::sendToServer( const char* buffer )
{
	if(send(sock, buffer, strlen(buffer), 0) < 0)
	{
	    perror("send()");
	    //exit(-1);
	}
}

void Client::receiveFromServer( char* buffer, int size )
{
	int n = 0;

	if((n = recv(sock, buffer, size - 1, 0)) < 0)
	{
	    perror("recv()");
	    //exit(-1);
	}

	if( n == 0 )
	{
		cout<<"server disconnected"<<endl;
		exit(-1);
	}

	buffer[n] = '\0';
}

Client::~Client() 
{
	end();

}

void Client::Deinitialize()
{
	closesocket(sock);
}








#endif // ANDROID