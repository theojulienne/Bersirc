/*************************************************************************
$Id$

Bersirc - A cross platform IRC client utilizing the Claro GUI Toolkit.
Copyright (C) 2004-2005 Theo P. Julienne and Nicholas S. Copeland

This application is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This application is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this application; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**************************************************************************/

#include "includes.h"

void b_identity_getautonick( int num, char *buf )
{
	XMLItem *nicks;
	XMLItem *nick;
	char *nicktxt;
	char *first = "Unknown";
	int a;
	
	nicks = c_xml_find_child( xidentity, "nicks" );
	
	for ( nick = nicks->child_head, a = 0; nick != 0; nick = nick->next, a++ )
	{
		nicktxt = c_xml_attrib_get( nick, "name" );
		
		if ( a == num )
		{
			strcpy( buf, nicktxt );
			return;
		}
		
		if ( a == 0 )
			first = nicktxt;
	}
	
	sprintf( buf, "%s[%d]", first, num-a+1 );
}

void b_handle_socket_lookup( CSocket *sock )
{
	BServerWindow *sw;
	
	if ( !( sw = b_find_server_by_sock( sock ) ) )
		return;
	
	b_swindow_printf( sw, BTV_ConnectionMsg, lang_phrase_quick( "connect_lookup" ) );
	
	sw->connected = 0;
}

void b_handle_socket_lookup_failed( CSocket *sock )
{
	BServerWindow *sw;
	
	if ( !( sw = b_find_server_by_sock( sock ) ) )
		return;
	
	b_swindow_printf( sw, BTV_ConnectionMsg, lang_phrase_quick( "connect_errorl" ) );
	
	sw->connected = 0;
}

void b_handle_socket_connecting( CSocket *sock )
{
	BServerWindow *sw;
	char buf[1024], ptmp[20];
	
	if ( !( sw = b_find_server_by_sock( sock ) ) )
		return;
	
	if ( !strcmp( sw->server_network, "" ) )
		sprintf( ptmp, "%s:%d", sw->server_name, sw->server_port );
	else
		sprintf( ptmp, "%s (%s:%d)", sw->server_network, sw->server_name, sw->server_port );
	
	lang_phrase_parse( buf, 1024, "connect_connecting", "server", ptmp, NULL );
	b_swindow_printf( sw, BTV_ConnectionMsg, buf );
	
	sw->connected = 0;
}

void b_handle_socket_connected( CSocket *sock )
{
	BServerWindow *sw;
	
	if ( !( sw = b_find_server_by_sock( sock ) ) )
		return;
	
	b_swindow_printf( sw, BTV_ConnectionMsg, lang_phrase_quick( "connect_connected_login" ) );
	
	sw->connected = 1;
}

void b_handle_socket_failed( CSocket *sock )
{
	BServerWindow *sw;
	
	if ( !( sw = b_find_server_by_sock( sock ) ) )
		return;
	
	b_swindow_printf( sw, BTV_ConnectionMsg, lang_phrase_quick( "connect_failed" ) );
	
	sw->connected = 0;
}

void b_handle_socket_disconnected( CSocket *sock )
{
	BServerWindow *sw;
	BChatWindow *cw;
	node_t *n;
	
	if ( !( sw = b_find_server_by_sock( sock ) ) )
		return;
	
	b_swindow_printf( sw, BTV_ConnectionMsg, lang_phrase_quick( "connect_disconnected" ) );
	
	sw->connected = 0;
	
	if ( !object_pending_destroy(sw->window) )
		b_server_update_title( sw );
	
	// clear all channel userlists to be clean
	LIST_FOREACH( n, sw->chat_windows.head )
	{
		cw = (BChatWindow *)n->data;
		
		if ( cw->type == B_CMD_WINDOW_CHANNEL )
		{
			cw->parted = 1;
			//c_listbox_clear( cw->userlist );
			c_tbl_empty( cw->users_table );
		}
	}
}

void b_server_connect( BServerWindow *server, char *hostname, int port, char *network )
{
	char username[64], realname[64];
	
	if ( server->sock->status != C_SOCK_DISCONNECTED )
		c_socket_close( server->sock );
	
	strcpy( server->server_network, network );
	strcpy( server->server_name, hostname );
	server->server_port = port;
	
	c_socket_connect( server->sock, hostname, port );
	
	strcpy( server->nickname, "" );
	
	if ( !strcasecmp( server->nickname, "" ) )
	{
		server->nickcount = 0;
		b_identity_getautonick( server->nickcount, server->nickname );
	}
	
	b_identity_attrib( "username", username );
	b_identity_attrib( "realname", realname );
	
	if ( strcmp( server->password, "" ) )
		b_server_printf( server, "PASS %s", server->password );
	b_server_printf( server, "USER %s %s %s :%s", username, "b", "c", realname );
	b_server_printf( server, "NICK %s", server->nickname );
	
	server->welcomed = 0;
	
}

void b_server_run_onconnect( BServerWindow *sw )
{
	char *item, *txt;
	BChatWindow *cw;
	
	if ( sw->welcomed == 1 )
		return;
	
	sw->welcomed = 1;
	
	b_server_printf( sw, "USERHOST :%s", sw->nickname );
	
	if ( sw->onconnect == 0 )
		return;
	
	txt = sw->onconnect;
	
	while ( ( item = strtok( txt, "," ) ) != NULL )
	{
		txt = NULL;
		
		cw = b_find_chat_by_dest( sw, item );
		
		if ( cw == 0 )
		{
			if ( item[0] == '#' || item[0] == '&' )
				b_server_printf( sw, "JOIN :%s", item );
			else
				cw = b_new_chat_window( sw, item, 0 );
		}
		
		if ( cw != 0 )
			widget_focus( OBJECT(cw->input) );
	}
	
	free( sw->onconnect );
	sw->onconnect = 0;
	sw->onconnect_len = 0;
}

void b_server_onconnect_goto( BServerWindow *sw, char *place )
{
	char *prefix = "";
	int a;
	
	a = sw->onconnect_len;
	
	sw->onconnect_len += strlen( place );
	
	if ( a != 0 ) {
		sw->onconnect_len += strlen( place ) + 1;
		prefix = ",";
	}
	
	sw->onconnect = realloc( sw->onconnect, sw->onconnect_len + 1 );
	if ( a == 0 )
		strcpy( sw->onconnect, "" );
	strcat( sw->onconnect, prefix );
	strcat( sw->onconnect, place );
}

void b_servers_run( )
{
	BServerWindow *win;
	int a;
	char buf[1024];
	node_t *n, *nn;
	
	LIST_FOREACH_SAFE( n, nn, bersirc->servers.head )
	{
		win = (BServerWindow *)n->data;
		
		if ( win->sock->status != C_SOCK_CONNECTED )
			continue;
		
		a = c_socket_numlines( win->sock );
	
		for ( ; a > 0; a-- )
		{
			c_socket_readline( win->sock, buf, 1024 );
			//c_btv_addline( win->content, buf, 0, BTV_ConnectionMsg );
			b_server_message( win, buf );
		}
	}
}

void b_setup_socket_handlers( BServerWindow *server )
{
	c_socket_set_handler( server->sock, C_SOCK_LOOKUP, b_handle_socket_lookup );
	c_socket_set_handler( server->sock, C_SOCK_LOOKUP_FAILED, b_handle_socket_lookup_failed );
	c_socket_set_handler( server->sock, C_SOCK_CONNECTING, b_handle_socket_connecting );
	c_socket_set_handler( server->sock, C_SOCK_CONNECTED, b_handle_socket_connected );
	c_socket_set_handler( server->sock, C_SOCK_FAILED, b_handle_socket_failed );
	c_socket_set_handler( server->sock, C_SOCK_DISCONNECTED, b_handle_socket_disconnected );
}

