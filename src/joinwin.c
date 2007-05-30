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

object_t *joinwin = 0, *btnjoin, *btncancel;
object_t *join_lbl_channel, *join_txt_channel;
object_t *join_lbl_key, *join_txt_key;

void b_joinwin_killed( )
{
	joinwin = 0;
}

event_handler( b_joinwin_close )
{
	widget_close( OBJECT(joinwin) );
}

event_handler( b_joinwin_join )
{
	BServerWindow *win;
	char *channel=0, *key=0, *cmd;

	win = b_server_find_active( );

	if ( !win )
		return;

	channel = textbox_get_text( join_txt_channel );
	key = textbox_get_text( join_txt_key );

	if ( strcmp ( channel, "" ) != 0 )
	{
		cmd = (char *)malloc ( 256 );
		if ( !cmd )
			return;

		strcpy( cmd, "/join " );
		strcat( cmd, channel );

		if ( strcmp( key, "") != 0 )
		{
			strcat( cmd, " " );			
			strcat( cmd, key );
		}

		b_user_command( win, cmd, 0 );
		free( cmd );
		widget_close( joinwin );
	}
}

event_handler( b_join_channel_txtchanged )
{
	char *channel;

	channel = textbox_get_text( join_txt_channel );

	if ( strcmp( channel, "" ) )
		widget_enable( OBJECT(btnjoin) );
	else
		widget_disable( OBJECT(btnjoin) );
}

event_handler( b_join_channel_enterpressed )
{
	int key = widget_get_notify_key( object, event );
	char *channel;

	if ( key != cKeyReturn )
		return;

	channel = textbox_get_text( join_txt_channel );
	
	if ( channel == 0 )
		return;

	b_joinwin_join( object, event );
}

void b_open_joinwin( )
{
	if ( joinwin == 0 )
	{
		joinwin = window_widget_create( bersirc->mainwin, new_bounds( -1, -1, 350, 115 ), cWindowModalDialog | cWindowCenterParent );
		window_set_icon( joinwin, b_icon( "channel_window" ) );
		window_set_title( joinwin, lang_phrase_quick( "join_channel" ) );
		object_addhandler( joinwin, "destroy", b_joinwin_killed );

		join_lbl_channel = label_widget_create_with_text( joinwin, new_bounds( 10, 20, 70, -1 ), 0, lang_phrase_quick( "channel" ) );
		join_txt_channel = textbox_widget_create( joinwin, new_bounds(95, 18, 245, -1), 0 );
		textbox_set_text( join_txt_channel, "" );
		
		join_lbl_key = label_widget_create_with_text( joinwin, new_bounds( 10, 50, 70, -1 ), 0, lang_phrase_quick( "connectwin_password" ) );
		join_txt_key = textbox_widget_create( joinwin, new_bounds(95, 48, 245, -1), cTextBoxTypePassword );
		textbox_set_text( join_txt_key, "" );
		
		widget_set_notify( OBJECT(join_txt_channel), cNotifyKey );
		object_addhandler( join_txt_channel, "key_down", b_join_channel_enterpressed );
		object_addhandler( join_txt_channel, "changed", b_join_channel_txtchanged );
		widget_focus( join_txt_channel );
		
		widget_set_notify( OBJECT(join_txt_key), cNotifyKey );
		object_addhandler( join_txt_key, "key_down", b_join_channel_enterpressed );
		
		btnjoin = button_widget_create_with_label( joinwin, new_bounds( 130, 85, 100, -1 ), 0, lang_phrase_quick( "join" ) );
		object_addhandler( btnjoin, "pushed", b_joinwin_join );
		widget_disable( btnjoin );
		
		btncancel = button_widget_create_with_label( joinwin, new_bounds( 240, 85, 100, -1 ), 0, lang_phrase_quick( "cancel" ) );
		object_addhandler( btncancel, "pushed", b_joinwin_close );
		
		window_show( joinwin );
	}
	else
	{
		widget_focus( joinwin );
	}
}

//
