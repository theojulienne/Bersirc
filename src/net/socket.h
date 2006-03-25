/*************************************************************************
$Id: socket.h 150 2005-08-10 01:03:47Z terminal $

Claro - A cross platform GUI toolkit which "makes sense".
Copyright (C) 2004 Theo Julienne and Gian Perrone

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**************************************************************************/

#ifdef ENV_CARBON
#define ENV_LINUXCOMPATSOCKS
#endif

#ifdef ENV_GTK
#define ENV_LINUXCOMPATSOCKS
#endif

#ifdef ENV_LINUXCOMPATSOCKS
// we need these libs
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <sys/time.h>
#endif

#define C_SOCK_SELECT_MSEC 100

#define C_SOCK_ERR_HOSTLOOKUP 1 // Could not resolve hostname
#define C_SOCK_ERR_SOCKCREATE 2 // Could not create socket
#define C_SOCK_ERR_CONNECTERROR 4 // Connection refused
#define C_SOCK_ERR_SOCKPARAM 8 // Could not set socket parameters
#define C_SOCK_ERR_CLOSED 16

enum CSocketState
{
	C_SOCK_DISCONNECTED=0,
	C_SOCK_LOOKUP,
	C_SOCK_LOOKUP_FAILED,
	C_SOCK_CONNECTING,
	C_SOCK_CONNECTED,
	C_SOCK_FAILED,
	C_SOCK_LISTENING,
	C_SOCK_CLIENTWAITING
};

typedef struct csocks
{
	int status;
	int error;
	
	char *read_buffer;
	int read_size;
	int read_lines;
	
	char *write_buffer;
	int write_size;
	
	void (*notify_lookup)( struct csocks *sock );
	void (*notify_lookup_failed)( struct csocks *sock );
	void (*notify_connecting)( struct csocks *sock );
	void (*notify_connected)( struct csocks *sock );
	void (*notify_disconnect)( struct csocks *sock );
	void (*notify_failed)( struct csocks *sock );
	void (*notify_listening)( struct csocks *sock );
	void (*notify_clientwaiting)( struct csocks *sock );
	
#ifdef ENV_WIN32
	SOCKET s;
	WORD sockVersion;
	WSADATA wsaData;
	int nret;
	LPHOSTENT hostEntry;
	SOCKADDR_IN serverInfo;
#endif

#ifdef ENV_LINUXCOMPATSOCKS
	int s;
	int nret;
	struct sockaddr_in serv_addr;
	struct hostent *server;
#endif

	struct csocks *next;
	struct csocks *prev;
	
	void *data;
} CSocket;

void c_socket_set_handler( CSocket *sock, int action, void *handler );

void c_socket_run( );
char *c_socket_hostname_to_ip( char *hostname );
void c_socket_close( CSocket *sock );
CSocket *c_socket_create( );
void c_socket_connect( CSocket *sock, char *hostname, int port );
void c_socket_destroy( CSocket *sock );
int c_socket_numlines( CSocket *sock );
int c_socket_readline( CSocket *sock, char *buffer, int size );
int c_socket_printf( CSocket *sock, char *fmt, ... );
void c_socket_listen( CSocket *sock, int port );
void c_socket_accept( CSocket *sock, CSocket *s );
CSocket *c_socket_list( );
void c_socket_close_real( CSocket *sock );
void c_socket_set_state( CSocket *sock, int state );
void c_socket_write( CSocket *sock, char *data, int size );

//
