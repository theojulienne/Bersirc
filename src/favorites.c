/*************************************************************************
$Id: berscore.c 156 2005-08-11 23:32:54Z terminal $

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

/* FIXME UPGRADE: upgrade BersircFavorite to list_t/node_t */

typedef struct bfav
{
	list_item_t *menuitem;
	XMLItem *xmlitem;
	
	struct bfav *next;
} BersircFavorite;

BersircFavorite *favs_head=0;

void b_store_favorite( list_item_t *m, XMLItem *x )
{
	BersircFavorite *fav;
	
	if ( !( fav = malloc( sizeof( BersircFavorite ) ) ) )
		return;
	
	fav->menuitem = m;
	fav->xmlitem = x;
	fav->next = favs_head;
	favs_head = fav;
}

event_handler( b_favorite_clicked )
{
	BersircFavorite *curr;
	char *server, *port, *place, *pass;
	int iport;
	BServerWindow *sw;
	BChatWindow *cw;
	
	for ( curr = favs_head; curr != 0; curr = curr->next )
	{
		if ( OBJECT(curr->menuitem) == object )
		{
			server = c_xml_attrib_get( curr->xmlitem, "server" );
			port = c_xml_attrib_get( curr->xmlitem, "port" );
			pass = c_xml_attrib_get( curr->xmlitem, "password" );
			place = c_xml_attrib_get( curr->xmlitem, "place" );
			
			if ( server == 0 || place == 0 )
				return;
			
			if ( port == 0 )
				port = "6667";
			
			if ( pass == 0 )
				pass = "";
			
			iport = atoi( port );
			
			sw = b_find_server_by_sname( server, 1 );
			cw = 0;
			
			if ( sw == 0 )
			{
				sw = b_find_server_by_sname( server, 0 );
				
				if ( sw == 0 )
				{
					// look for disconnected one
					sw = b_find_server_by_conn( 0 );
					
					if ( sw == 0 )
					{
						// open new server window!
						sw = b_new_server_window( 0 );
					}
				}
				
				// tell sw where to go after connect
				b_server_onconnect_goto( sw, place );
				
				// set password
				strcpy( sw->password, pass );
				
				// connect sw to server
				b_server_connect( sw, server, iport, "" );
			}
			else
			{
				// server window open.
				
				cw = b_find_chat_by_dest( sw, place );
				
				if ( cw == 0 )
				{
					// open new chat window
					if ( place[0] == '#' || place[0] == '&' )
						b_server_printf( sw, "JOIN %s", place );
					else
						cw = b_new_chat_window( sw, place, 0 );
				}
				
				if ( cw != 0 )
					c_widget_focus( cw->input ); /* PORTFIX */
			}
		}
	}
}

void menu_setup_favorites( object_t *menu, list_item_t *fav, XMLItem *head )
{
	XMLItem *curr;
	list_item_t *newi;
	image_t *icon;
	char *place;
	
	if ( head == 0 || head->child_head == 0 )
		return; // FIXME: do something else than leave an empty menu

	for ( curr = head->child_head; curr != 0; curr = curr->next ) {
		if ( !strcasecmp( curr->name, "directory" ) )
		{
			newi = menubar_append_item( menu, fav, b_icon( "folder_closed" ), c_xml_attrib_get( curr, "name" ) );
			menu_setup_favorites( menu, newi, curr );
		}
		else if ( !strcasecmp( curr->name, "bookmark" ) )
		{
			icon = b_icon( "channel_window" );
			
			place = c_xml_attrib_get( curr, "place" );
			if ( place != 0 && place[0] != '#' && place[0] != '&' )
				icon = b_icon( "query_window" );
			
			newi = menubar_append_item( menu, fav, icon, c_xml_attrib_get( curr, "name" ) );
			b_store_favorite( newi, curr );
			object_addhandler( OBJECT(newi), "pushed", b_favorite_clicked );
		}
	}
}


