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

int b_mode_handle_power( B_MODE_HANDLER_PARMS )
{
	BUserStore *user;
	char newmodes[16];
	int a, b;
	
	// BServerWindow *server, BChatWindow *chanwin, char dir, char mode, char *channel, char *victim
	
	if ( ( user = b_chat_user_find_nick( chanwin, victim ) ) == 0 )
		return 0;
	
	strcpy( newmodes, user->modes );
	
	if ( dir == '+' && strchr( user->modes, mode ) == 0 )
	{
		sprintf( newmodes, "%s%c", user->modes, mode );
	}
	else if ( dir == '-' )
	{
		memset( &newmodes, 0, 16 );
		for ( a = 0, b = 0; a < strlen( user->modes ); a++ )
		{
			if ( user->modes[a] != mode )
			{
				newmodes[b] = user->modes[a];
				b++;
			}
		}
	}
	
	// save. remember, the original will be used if nothing happened.
	strcpy( user->modes, newmodes );
	
	b_userstore_updated( chanwin, user, 1 );
	
	return 0;
}

int b_mode_handle_null( B_MODE_HANDLER_PARMS )
{
	return 0;
}

int b_mode_handle_key( B_MODE_HANDLER_PARMS )
{
	if ( dir == '-' )
		strcpy( chanwin->key, "" );
	else
		strcpy( chanwin->key, victim );
	
	return 0;
}

void b_mode_handle_any( B_MODE_HANDLER_PARMS )
{
	int a;
	node_t *n, *nn;
	channel_mode_t *cmode;
	
	// BServerWindow *server, BChatWindow *chanwin, char dir, char mode, char *channel, char *victim

	if ( dir == '-' )
	{
		// remove that row
		LIST_FOREACH_SAFE( n, nn, chanwin->channel_modes.head )
		{
			cmode = (channel_mode_t *)n->data;
			
			if ( cmode->mode == mode )
			{
				free( cmode->victim );
				free( n->data );
				node_del( n, &chanwin->channel_modes );
				node_free( n );
			}
		}
	}
	else
	{
		// firstly, remove the row (just in case) :p
		b_mode_handle_any( server, chanwin, '-', mode, channel, victim );
		
		// now add it
		if ( !( cmode = malloc(sizeof(channel_mode_t)) ) )
			return;
		
		n = node_create( );
		node_add( cmode, n, &chanwin->channel_modes );
		
		cmode->mode = mode;
		cmode->victim = (victim?strdup( victim ):strdup(""));
	}
	
	b_chat_update_title( chanwin );
}

/**
 * Adds a mode to the server's channel mode list.
 */
void b_server_add_cmode( bserver_t *server, char mode, int flags )
{
	node_t *n;
	channel_mode_handler_t *cmode;
	
	if ( !( cmode = malloc(sizeof(channel_mode_handler_t)) ) )
		return;
	
	n = node_create( );
	
	node_add( cmode, n, &server->channel_mode_handlers );
	
	cmode->mode = mode;
	cmode->flags = flags;
	cmode->handler = b_mode_handle_null;
	
	switch ( mode )
	{
		case 'k':
			cmode->handler = b_mode_handle_key;
			break;
		case 'o':
		case 'a':
		case 'h':
		case 'q':
		case 'v':
			cmode->handler = b_mode_handle_power;
			break;
	}
}

/**
 * Clears all server channel modes, ready for a new lot
 */
void b_server_clear_cmodes( bserver_t *server, int mask )
{
	node_t *n, *nn;
	channel_mode_handler_t *cmode;
	
	LIST_FOREACH_SAFE( n, nn, server->channel_mode_handlers.head )
	{
		cmode = (channel_mode_handler_t *)n->data;
		
		if ( (cmode->flags & mask) != mask )
			continue;
		
		free( n->data );
		node_del( n, &server->channel_mode_handlers );
		node_free( n );
	}
}

channel_mode_handler_t *b_server_mode_find( bserver_t *server, char mode )
{
	node_t *n, *nn;
	channel_mode_handler_t *cmode;
	
	LIST_FOREACH_SAFE( n, nn, server->channel_mode_handlers.head )
	{
		cmode = (channel_mode_handler_t *)n->data;
		
		if ( cmode->mode == mode )
			return cmode;
	}
	
	return 0;
}

/**
 * Handles a channel mode change.
 */
int b_channel_mode_handle( bserver_t *server, bchannel_t *cw, char dir, char mode, char *channel, char *victim )
{
	channel_mode_handler_t *cmode = b_server_mode_find( server, mode );
	
	if ( !cmode )
		return 0;
	
	(*cmode->handler)( server, cw, dir, mode, channel, victim );
	
	if ( cmode->flags & bModeRemember )
	{
		b_mode_handle_any( server, cw, dir, mode, channel, victim );
	}
	
	if ( dir == '+' && (cmode->flags & bModeHasParamOnSet) )
		return 1;
	else if ( dir == '-' && (cmode->flags & bModeHasParamOnUnset) )
		return 1;
	
	return 0;
}

/**
 * Empties the specified channel's mode list.
 */
void b_channel_mode_list_clean( bchannel_t *cw )
{
	node_t *n, *nn;
	
	LIST_FOREACH_SAFE( n, nn, cw->channel_modes.head )
	{
		free( n->data );
		node_del( n, &cw->channel_modes );
		node_free( n );
	}
}


