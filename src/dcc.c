#include "includes.h"

void dcc_failed( CSocket *sock );

void dcc_send_init( CSocket *sock )
{
	CSocket *s = c_socket_create();
	
	c_socket_accept( sock, s );
	
	s->data = sock->data;
	((struct DCCData*)s->data)->state = 2;
	((struct DCCData*)s->data)->startTime = time( 0 );
	
	c_socket_close_real( sock );
	c_socket_destroy( sock );
	
	c_socket_set_handler( s, C_SOCK_LISTENING, &dcc_donothing );
	c_socket_set_handler( s, C_SOCK_DISCONNECTED, &dcc_donothing );
	c_socket_set_handler( s, C_SOCK_CLIENTWAITING, &dcc_send );
	c_socket_set_state( s, C_SOCK_LISTENING );
	
	dcc_send( s );
}

void dcc_send( CSocket *sock )
{
	int a;
	unsigned long size;
	char bb[4];
	int buffer_size;
	char *buffer;
	
	if ( ((struct DCCData*)sock->data)->packetSize != 0 && ( a = recv( sock->s, bb, 4, 0 ) ) == 4 )
	{
		size = ntohl(*((unsigned long *) bb));
		((struct DCCData*)sock->data)->position = size;
	}
				
	// End of Initial Checks
	if ( a == 0 ) // no data sent, socket closed, lets end it
	{
		dcc_close( sock );
		return;
	}	
	
	if ( ((struct DCCData*)sock->data)->position >= ((struct DCCData*)sock->data)->fileSize )
	{
		dcc_close( sock );
		return;
	}
	
	else
	{
		FILE *file;
		
		if( ((struct DCCData*)sock->data)->fileSize <= ((struct DCCData*)sock->data)->position + ((struct DCCData*)sock->data)->dpacketSize )
			((struct DCCData*)sock->data)->packetSize = ((struct DCCData*)sock->data)->fileSize - ((struct DCCData*)sock->data)->position;
		else
			((struct DCCData*)sock->data)->packetSize = ((struct DCCData*)sock->data)->dpacketSize;
			
		buffer_size = ((struct DCCData*)sock->data)->packetSize;
		
		buffer = malloc( buffer_size + 1 );
			
		file = fopen( ((struct DCCData*)sock->data)->file, "rb" );
		fseek( file, ((struct DCCData*)sock->data)->position, SEEK_SET );
		a = fread( buffer, 1, buffer_size, file );
		buffer_size = a;
		fclose( file );
		
		c_socket_write( sock, buffer, buffer_size );
		free( buffer );
		
		c_socket_set_state( sock, C_SOCK_LISTENING );
	}
}

void dcc_recv_init( CSocket *sock )
{
#ifdef ENV_WIN32
	sock->serverInfo.sin_addr.S_un.S_addr = ((struct DCCData*)sock->data)->ip;
#endif

	c_socket_set_handler( sock, C_SOCK_CONNECTING, &dcc_donothing );
	c_socket_set_handler( sock, C_SOCK_LISTENING, &dcc_donothing );
	c_socket_set_handler( sock, C_SOCK_DISCONNECTED, &dcc_donothing );
	c_socket_set_handler( sock, C_SOCK_CLIENTWAITING, &dcc_recv );
	c_socket_set_handler( sock, C_SOCK_FAILED, &dcc_failed );
	
#ifdef ENV_WIN32
	c_socket_connect( sock, inet_ntoa( sock->serverInfo.sin_addr ), ((struct DCCData*)sock->data)->position );
#endif
	((struct DCCData*)sock->data)->position = 0;
	((struct DCCData*)sock->data)->state = 4;
	((struct DCCData*)sock->data)->startTime = time( 0 );
	
	c_socket_set_state( sock, C_SOCK_LISTENING );
}

void dcc_recv( CSocket *sock )
{
	int a;
	unsigned long count = 0;
	char *temp, *buffer, bb[4];
	FILE *file;
	
	buffer = (char *)malloc( 1024 );
	a = recv( sock->s, buffer, 1024, 0 );
				
	if ( a == 0 ) // no data sent (initially), socket closed, lets end it
	{
		dcc_close( sock );
		return;
	}
	
	temp = (char *)malloc( 256 );
	sprintf( temp, "downloads\\%s", ((struct DCCData*)sock->data)->file );
	
	do
	{
		file = fopen( temp, "ab" );
		if ( file )
		{
			fwrite( buffer, 1, a, file );
			fclose( file );
		}
		count += a;
	}
	while ( ( a = recv( sock->s, buffer, 1024, 0 ) ) > 0 );
	
	free( temp );
	free( buffer );
	
	((struct DCCData*)sock->data)->position += count;
	count = htonl( ((struct DCCData*)sock->data)->position );
	bb[0] = (char)(0xff & (count >> 0 ));
	bb[1] = (char)(0xff & (count >> 8 ));
	bb[2] = (char)(0xff & (count >> 16 ));
	bb[3] = (char)(0xff & (count >> 24 ));

	c_socket_write( sock, bb, 4 );
	
	c_socket_set_state( sock, C_SOCK_LISTENING );
}

void dcc_donothing( CSocket *sock )
{
	// nothing :)
}

void dcc_failed( CSocket *sock )
{
	lang_phrase_parse( lang_tmp_buf, 1024, "dcc_recv_failed", "nick", ((struct DCCData*)sock->data)->name, 0 );
	
	b_window_printf( b_active_window( ), BTV_CTCP, "%s", lang_tmp_buf );
	
	// clean up
	dcc_close( sock );
}

void dcc_close( CSocket *sock )
{
	free( ((struct DCCData*)sock->data)->file );
	free( ((struct DCCData*)sock->data)->name );
	free( sock->data );
	sock->data = 0;
	c_socket_close_real( sock );
	c_socket_destroy( sock );
}

