/*************************************************************************
$Id: socket.c 165 2005-08-19 00:26:34Z terminal $

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

#include "../includes.h"

/***

 This really should be made part of Claro in the way originally intended,
 which was to have "claro.dll", "clarogt.dll", "clarost.dll", etc.

 ***/

CSocket *list_sock_head = 0;

void c_socket_set_handler( CSocket *sock, int state, void *handler )
{
	if ( state == C_SOCK_DISCONNECTED )
		sock->notify_disconnect = handler;
	else if ( state == C_SOCK_LOOKUP )
		sock->notify_lookup = handler;
	else if ( state == C_SOCK_LOOKUP_FAILED )
		sock->notify_lookup_failed = handler;
	else if ( state == C_SOCK_CONNECTING )
		sock->notify_connecting = handler;
	else if ( state == C_SOCK_CONNECTED )
		sock->notify_connected = handler;
	else if ( state == C_SOCK_FAILED )
		sock->notify_failed = handler;
	else if ( state == C_SOCK_LISTENING )
		sock->notify_listening = handler;
	else if ( state == C_SOCK_CLIENTWAITING )
		sock->notify_clientwaiting = handler;
}

void c_socket_set_state( CSocket *sock, int state )
{
	void (*notify)( struct csocks *sock ) = 0;
	
	if ( sock->status == state )
		return;
	
	sock->status = state;
	
	if ( state == C_SOCK_DISCONNECTED )
		notify = sock->notify_disconnect;
	else if ( state == C_SOCK_LOOKUP )
		notify = sock->notify_lookup;
	else if ( state == C_SOCK_LOOKUP_FAILED )
	{
		notify = sock->notify_lookup_failed;
		state = C_SOCK_FAILED;
	}
	else if ( state == C_SOCK_CONNECTING )
		notify = sock->notify_connecting;
	else if ( state == C_SOCK_CONNECTED )
		notify = sock->notify_connected;
	else if ( state == C_SOCK_FAILED )
		notify = sock->notify_failed;
	else if ( state == C_SOCK_LISTENING )
		notify = sock->notify_listening;
	else if ( state == C_SOCK_CLIENTWAITING )
		notify = sock->notify_clientwaiting;
	else
		return;
	
	if ( notify == 0 )
	{
		printf( "%d has no handler - this isn't a big deal, really..\n", state );
		return;
	}
	
	(*notify)( sock );
}

void c_socket_close_real( CSocket *sock )
{
#ifdef ENV_LINUX
		close( sock->s );
#endif
#ifdef ENV_WIN32
		closesocket( sock->s );
#endif
	
	if ( sock->read_buffer != 0 )
		free( sock->read_buffer );
	sock->read_buffer = 0;
	sock->read_size = 0;
	
	if ( sock->write_buffer != 0 )
		free( sock->write_buffer );
	sock->write_buffer = 0;
	sock->write_size = 0;

	c_socket_set_state( sock, C_SOCK_DISCONNECTED );
}

void c_socket_close( CSocket *sock )
{
	// do pending buffer writes etc
	c_socket_run( );
	
	c_socket_close_real( sock );
}

CSocket *c_socket_create( )
{
	CSocket *sock = (CSocket *)malloc( sizeof( CSocket ) );
	
	memset( sock, 0, sizeof( CSocket ) );
	
	sock->next = list_sock_head;
	if ( list_sock_head != 0 )
		list_sock_head->prev = sock;
	list_sock_head = sock;
	
	sock->read_buffer = 0;
	sock->read_lines = 0;
	sock->read_size = 0;
	
	sock->write_buffer = 0;
	sock->write_size = 0;
	
	sock->data = 0;
	
#ifdef ENV_WIN32
	sock->sockVersion = MAKEWORD( 1, 1 );

	WSAStartup( sock->sockVersion, &sock->wsaData );
#endif

	return sock;
}

int c_socket_numlines( CSocket *sock )
{
	return sock->read_lines;
}

void c_socket_write( CSocket *sock, char *data, int size )
{
	int b;
	
	if ( sock->status == C_SOCK_FAILED || sock->status == C_SOCK_DISCONNECTED )
		return;
	
	b = sock->write_size;
	
	sock->write_size += size;
	
	sock->write_buffer = (char *)realloc( sock->write_buffer, sock->write_size + 1 );
	
	memcpy( &sock->write_buffer[b], data, size );
	sock->write_buffer[sock->write_size] = 0;
}

int c_socket_printf( CSocket *sock, char *fmt, ... )
{
	va_list args;
	char buf[16384];
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	c_socket_write( sock, buf, strlen( buf ) );
	
	return 1;
}

int c_socket_readline( CSocket *sock, char *buffer, int size )
{
	int a, b;
	char *ptr;
	char *cpy;
	
	if ( sock->read_lines < 1 || sock->read_buffer == 0 )
		return 0;
	
	a = b = 0;
	
	ptr = sock->read_buffer;
	
	while ( b < sock->read_size && ptr[0] != '\r' && ptr[0] != '\n' )
	{
		buffer[a] = ptr[0];
		
		b++;
		ptr++;
		
		if ( a+1 < size )
			a++;
	}
	
	buffer[a] = 0;
	
	if ( b < sock->read_size && ptr[0] == '\r' )
	{
		ptr++; b++;
	}
	
	if ( b < sock->read_size && ptr[0] == '\n' )
	{
		ptr++; b++;
	}
	
	sock->read_size -= b;
	
	cpy = (char *)malloc( sock->read_size + 1 );
	memcpy( cpy, ptr, sock->read_size );
	
	free( sock->read_buffer );
	
	sock->read_buffer = cpy;
	
	sock->read_lines -= 1;
	
	return 1;
}

void c_socket_run( )
{
	CSocket *curr;
	fd_set fdRead, fdWrite, fdExcept;
	struct timeval m;
	int a, b, c, d;
	char bb[1024];
	char *cpy;
	
	for ( curr = list_sock_head; curr != 0; curr = curr->next )
	{
		if ( curr->status == C_SOCK_DISCONNECTED || curr->status == C_SOCK_FAILED )
			continue;
		
		FD_ZERO( &fdRead );
		FD_ZERO( &fdWrite );
		FD_ZERO( &fdExcept );
		FD_SET( curr->s, &fdRead );
		FD_SET( curr->s, &fdWrite );
		FD_SET( curr->s, &fdExcept );
	
		m.tv_sec = C_SOCK_SELECT_MSEC / 1000;
		m.tv_usec = C_SOCK_SELECT_MSEC % 1000;
	
		a = select( curr->s + 1, &fdRead, &fdWrite, &fdExcept, &m );
			
		if ( a == 0 ) // nothing new has happened
			continue;
	
		if ( FD_ISSET( curr->s, &fdExcept ) )
		{
			// whee! an error!
			curr->error = C_SOCK_ERR_CLOSED;
			c_socket_set_state( curr, C_SOCK_FAILED );
			c_socket_close_real( curr );
			continue;
		}
		
		if ( curr->status == C_SOCK_LISTENING && FD_ISSET( curr->s, &fdRead ) )
		{
			c_socket_set_state( curr, C_SOCK_CLIENTWAITING );
			break;
		}
		
		if ( curr->status == C_SOCK_CONNECTING && FD_ISSET( curr->s, &fdWrite ) )
		{
			c_socket_set_state( curr, C_SOCK_CONNECTED );
		}
		
		if ( FD_ISSET( curr->s, &fdRead ) )
		{
			bb[0] = 0;
			c = 0;
			d = 0;
			
			while ( ( a = recv( curr->s, bb, 1024, 0 ) ) > 0 )
			{
				d++;
				
				for ( b = 0; b < a; b++ )
					if ( bb[b] == '\n' )
						c++;
				
				b = curr->read_size;
				
				curr->read_size += a;
				curr->read_buffer = (char *)realloc( curr->read_buffer, curr->read_size + 1 );
				
				memcpy( &curr->read_buffer[b], &bb, a );
				curr->read_buffer[curr->read_size] = 0;
			}
			
			if ( d == 0 )
			{
				// we got NO data! disconnected.
				curr->error = C_SOCK_ERR_CLOSED;
				c_socket_close_real( curr );
				continue;
			}
			
			curr->read_lines += c;
		}
		
		if ( FD_ISSET( curr->s, &fdWrite ) )
		{
			if ( curr->write_size > 0 )
			{
				// got data to write. we're going to write as much as possible.
				// if some doesn't get written, we can try again next itteration.
				
				a = send( curr->s, curr->write_buffer, curr->write_size, 0 );
				
				curr->write_size -= a;
				
				if ( curr->write_size == 0 )
				{
					free( curr->write_buffer );
					curr->write_buffer = 0;
				}
				else
				{
					cpy = (char *)malloc( curr->write_size + 1 );
					memcpy( cpy, &curr->write_buffer[a], curr->write_size );
					cpy[curr->write_size] = 0;
					
					free( curr->write_buffer );
					curr->write_buffer = cpy;
				}
			}
		}
	}
}

char *c_socket_hostname_to_ip( char *hostname )
{
#ifdef ENV_WIN32
	LPHOSTENT hostEntry;
	struct in_addr *a;

	hostEntry = gethostbyname( hostname );
	
	if ( !hostEntry )
		return hostname;
	
	a = (struct in_addr *)*hostEntry->h_addr_list;
	
	return inet_ntoa( *a );
#else
	return "";
#endif
}

void c_socket_connect( CSocket *sock, char *hostname, int port )
{
#ifdef ENV_WIN32
	int a = 0;
	sock->hostEntry = gethostbyname( hostname );

	c_socket_set_state( sock, C_SOCK_DISCONNECTED );

	if ( !sock->hostEntry )
	{
		c_socket_set_state( sock, C_SOCK_LOOKUP_FAILED );
		c_socket_set_state( sock, C_SOCK_FAILED );
		sock->error = C_SOCK_ERR_HOSTLOOKUP;
		return;
	}

	sock->s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( sock->s == INVALID_SOCKET ) {
		c_socket_set_state( sock, C_SOCK_FAILED );
		sock->error = C_SOCK_ERR_SOCKCREATE;
		return;
	}

	sock->serverInfo.sin_family = AF_INET;
	sock->serverInfo.sin_addr = *((LPIN_ADDR)*sock->hostEntry->h_addr_list);
	sock->serverInfo.sin_port = htons( port );

	a = 1;
	if( ioctlsocket( sock->s, FIONBIO, (unsigned long *)&a ) == -1 ) {
		c_socket_set_state( sock, C_SOCK_FAILED );
		sock->error = C_SOCK_ERR_SOCKPARAM;
		return;
	}

	connect( sock->s, (LPSOCKADDR)&sock->serverInfo, sizeof( struct sockaddr ) );
#endif

#ifdef ENV_LINUXCOMPATSOCKS
	sock->server = gethostbyname( hostname );
	
	if ( !sock->server )
	{
		c_socket_set_state( sock, C_SOCK_LOOKUP_FAILED );
		c_socket_set_state( sock, C_SOCK_FAILED );
		sock->error = C_SOCK_ERR_HOSTLOOKUP;
		return;
	}
	
	sock->s = socket( AF_INET, SOCK_STREAM, 0 );
	
	if ( sock->s < 0 ) {
		c_socket_set_state( sock, C_SOCK_FAILED );
		sock->error = C_SOCK_ERR_SOCKCREATE;
		return;
	}
	
	bzero( (char *) &sock->serv_addr, sizeof( sock->serv_addr ) );
	sock->serv_addr.sin_family = AF_INET;
	bcopy( (char *)sock->server->h_addr, (char *)&sock->serv_addr.sin_addr.s_addr, sock->server->h_length );
	sock->serv_addr.sin_port = htons( port );
	
	if ( fcntl( sock->s, F_SETFL, O_NONBLOCK ) == -1 ) {
		c_socket_set_state( sock, C_SOCK_FAILED );
		sock->error = C_SOCK_ERR_SOCKPARAM;
		return;
	}
	
	connect( sock->s, (struct sockaddr *)&sock->serv_addr, sizeof( sock->serv_addr) );
#endif

	c_socket_set_state( sock, C_SOCK_CONNECTING );
	
	sock->read_buffer = 0;
	sock->write_buffer = 0;
	
	sock->read_size = 0;
	sock->write_size = 0;

	return;
}

void c_socket_listen( CSocket *sock, int port )
{
	sock->s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	
#ifdef ENV_WIN32
	sock->serverInfo.sin_family = AF_INET;
	sock->serverInfo.sin_addr.S_un.S_addr = INADDR_ANY;
	sock->serverInfo.sin_port = htons( port );
	
	bind( sock->s, (SOCKADDR*)&sock->serverInfo, sizeof( sock->serverInfo ) );
#endif
	listen( sock->s, SOMAXCONN );
	
	c_socket_set_state( sock, C_SOCK_LISTENING );
}

void c_socket_accept( CSocket *sock , CSocket *s )
{
	s->s = accept( sock->s, 0, 0 );
}

CSocket *c_socket_list( )
{
	return list_sock_head;
}

void c_socket_destroy( CSocket *sock )
{
	if ( sock->status != C_SOCK_DISCONNECTED )
		c_socket_close( sock );
	
	if ( list_sock_head == sock ) {
		list_sock_head = sock->next;
	}
	
	if ( sock->prev != 0 )
		sock->prev->next = sock->next;
	
	if ( sock->next != 0 )
		sock->next->prev = sock->prev;
	
	free( sock );
}

//
