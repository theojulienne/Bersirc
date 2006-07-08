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

//BServerWindow *server_head = 0;

BServerWindow *b_find_server_by_widget( CWidget *w )
{
	BServerWindow *win;
	node_t *n;
	
	LIST_FOREACH( n, bersirc->servers.head )
	{
		win = (BServerWindow *)n->data;
		
		if ( win->window == w || win->content == w || win->input == w || win->conmenu.menu == w )
			return win;
	}
	
	return NULL;
}

BServerWindow *b_find_server_by_sock( CSocket *sock )
{
	BServerWindow *curr;
	node_t *n;

	LIST_FOREACH( n, bersirc->servers.head )
	{
		curr = (BServerWindow *)n->data;
		
		if ( curr->sock == sock )
			return curr;
	}
	
	return NULL;
}

BServerWindow *b_find_server_by_sname( char *data, int connected )
{
	BServerWindow *win;
	node_t *n;
	
	LIST_FOREACH( n, bersirc->servers.head )
	{
		win = (BServerWindow *)n->data;
		
		if ( !strcasecmp( win->servername, data ) )
		{
			if ( connected == 1 && win->connected == 0 )
				continue;
			
			return win;
		}
	}
	
	return NULL;
}

BServerWindow *b_find_server_by_conn( int connected )
{
	BServerWindow *win;
	node_t *n;
	
	LIST_FOREACH( n, bersirc->servers.head )
	{
		win = (BServerWindow *)n->data;
		
		if ( win->connected == connected )
			return win;
	}
	
	return NULL;
}

event_handler( b_server_content_focus )
{
	BServerWindow *sw = b_find_server_by_widget( object );
	
	if ( sw == 0 || sw->input == 0 )
		return;
	
	widget_focus( sw->input );
	
	b_taskbar_redraw( );
}

event_handler( b_server_win_destroy )
{
	BServerWindow *sw = b_find_server_by_widget( object );
	BChatWindow *curr;
	node_t *n, *tn;
	
	if ( sw == 0 )
		return;
	
	b_server_printf( sw, "QUIT :%s", b_get_quit_message( ) );
	c_socket_close( sw->sock );
	
	// remove from treeview
	if ( !object_pending_destroy(bersirc->treeview) )
		treeview_remove_row( bersirc->treeview, sw->tv_item );
	
	LIST_FOREACH_SAFE( n, tn, sw->chat_windows.head )
	{
		curr = (BChatWindow *)n->data;
		curr->server = 0;
		
		if ( !object_pending_destroy(curr->window) )
			widget_close( curr->window );
		
		node_del( n, &sw->chat_windows );
		node_free( n );
	}
	
	n = node_find( sw, &bersirc->servers );
	node_del( n, &bersirc->servers );
	node_free( n );
	
	// finally, get rid of the structure
	free( sw );
	
	b_taskbar_redraw( );
}

event_handler( b_server_enter_press )
{
	BServerWindow *win;
	
	event->handled = 1;
	
	if ( !strcmp( textbox_get_text(object), "" ) )
		return;
	
	if ( ( win = b_find_server_by_widget( object->parent ) ) == NULL )
		return;
	
	if ( b_user_command( win, textbox_get_text(object), 0 ) <= 0 )
		return;
	
	b_handle_enter_recall( object, win );
	
	c_text_settext( object, "" );
	//c_send_event( win->content->w, C_EVENT_PREDRAW, 0 );
}

void b_server_update_title( BServerWindow *server )
{
	char buf[1024];
	char *name;
	
	if ( server->connected == 0 )
		sprintf( buf, lang_phrase_quick( "status_title_idle" ) );
	else
		lang_phrase_parse( buf, 1024, "status_title_conn", "nick", server->nickname, "mode", server->umode, "server", server->servername, NULL );
	
	workspace_window_set_title( server->window, buf );
	b_taskbar_redraw( );
	
	name = "Status";
		
	if ( server->connected == 1 )
		name = server->servername;
	
	list_widget_edit_row( bersirc->treeview, server->tv_item, 1, name, -1 );
}

int b_server_printf( BServerWindow *server, char *fmt, ... )
{
	va_list args;
	char buf[16384];
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	c_socket_printf( server->sock, "%s\r\n", buf );
	
	return 1;
}

int b_swindow_printf( BServerWindow *server, int colour, char *fmt, ... )
{
	va_list args;
	char buf[16384];
	int of;
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	c_btv_addline( server->content, buf, 0, colour );
	
	of = server->taskbar_flags;
	
	if ( colour == BTV_Message || colour == BTV_Action )
	{
		if ( b_get_option_bool( xidentity, "general", "opt_gen_flash_on_message" ) )
		{
			c_window_flash( mainwin, 0, b_get_option_int( xidentity, "general", "opt_gen_flash_times" ) );
		}
		
		server->taskbar_flags |= 4;
	}
	else
		server->taskbar_flags |= 2;
	
	if ( of != server->taskbar_flags )
		b_taskbar_redraw( );
	
	return 1;
}

BServerWindow *b_server_find_active( )
{
	CWidget *w;
	BServerWindow *sw;
	BChatWindow *cw;
	
	w = c_workspace_get_active( bersirc->workspace );
	
	if ( w == 0 )
	{
		return 0;
	}
	
	sw = b_find_server_by_widget( w );
	
	if ( sw != 0 )
		return sw;
	
	cw = b_find_chat_by_widget( w );
	
	if ( cw != 0 )
		return cw->server;
	
	return 0;
}


BServerWindow *b_new_server_window( int flags )
{
	BServerWindow *server;
	object_t *line;
	node_t *n;
	char lt_str[256];
	
	server = (BServerWindow *)malloc( sizeof( BServerWindow ) );
	
	memset( server, 0, sizeof( BServerWindow ) );
	
	list_create( &server->chat_windows );
	list_create( &server->channel_mode_handlers );
	
	server->type = B_CMD_WINDOW_STATUS;
	
	n = node_create( );
	node_add( server, n, &bersirc->servers );
	
	server->recall_newest = 0;
	server->recall_shown = 0;
	server->recall_mode = 0;
	
	strcpy( server->servername, "unknown" );
	strcpy( server->password, "" );
	strcpy( server->umode, "" );
	
	bounds_t *b = new_bounds(-1,-1,600,400);
	server->window = workspace_window_widget_create( bersirc->workspace, b, 0 );
	sprintf( lt_str, "[_content|(%d)scrollbar][{1}line][{20}input]", scrollbar_get_sys_width( ) );
	server->layout = layout_create( server->window, lt_str, *b, 20, 20 );
	
	object_addhandler( server->window, "destroy", b_server_win_destroy );
	object_addhandler( server->window, "focus", b_server_content_focus );
	object_addhandler( server->window, "minimized", b_window_minimised );
	object_addhandler( server->window, "maximized", b_window_maximised );
	object_addhandler( server->window, "restored", b_window_restored );
	
#if 0 /* PORTHACK */
	//c_new_event_handler( server->window, C_EVENT_RESIZE, b_server_win_resize );
	c_new_event_handler( server->window, C_EVENT_DESTROY, b_server_win_destroy );
	c_new_event_handler( server->window, C_EVENT_ONFOCUS, b_server_content_focus );
	c_new_event_handler( server->window, C_EVENT_MINIMIZED, b_window_minimised );
	c_new_event_handler( server->window, C_EVENT_MAXIMIZED, b_window_maximised );
	c_new_event_handler( server->window, C_EVENT_RESTORED, b_window_restored );
#endif
	
	workspace_window_set_icon( server->window, b_icon( "status_window" ) );
	
	server->content = (object_t *)ircview_widget_create( server->window, lt_bounds(server->layout,"content") );
	object_addhandler( server->content, "focus", b_server_content_focus );
	
	server->scroll = scrollbar_widget_create( server->window, lt_bounds(server->layout,"scrollbar"), cScrollbarVertical );
	ircview_set_scrollbar( server->content, server->scroll );
	
	line = canvas_widget_create( server->window, lt_bounds(server->layout,"line"), 0 );
	object_addhandler( line, "redraw", b_draw_line_canvas );
	
	server->input = textbox_widget_create( server->window, lt_bounds(server->layout,"input"), cWidgetNoBorder );
	widget_set_notify( WIDGET(server->input), cNotifyKey );
	
	object_addhandler( server->input, "key_down", b_input_key_press );
	object_addhandler( server->input, "enter_press", b_server_enter_press );
#if 0 /* PORTHACK */
	c_new_event_handler( server->input, C_EVENT_MOUSE_WHEEL, c_btv_handle_mouse_wheel );
#endif
	
	/* config fonts */
	b_widget_set_font( server->content, "server-buffer" );
	b_widget_set_font( server->input, "server-input" );
	
	server->sock = c_socket_create( );
	b_setup_socket_handlers( server );
	
	if ( flags & 1 )
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "welcome_splash", "client", "\002Bersirc " BERSIRC_VERSION "\002", NULL );
		c_btv_addline( server->content, lang_tmp_buf, 0, BTV_ClientWelcome );
	}
	
	b_taskbar_redraw( );
	
	server->tv_queries = 0;
	server->tv_channels = 0;
	
	// add to the treeview
	server->tv_item = treeview_append_row( bersirc->treeview, 0, b_icon("server"), "Status" );
	server->tv_item->appdata = (void *)server;
	list_item_set_font_extra( server->tv_item, cFontWeightBold, cFontSlantNormal, cFontDecorationNormal );
	
	b_window_create_context_menu( server );
	
	b_server_update_title( server );
	
	workspace_window_show( server->window );
	
	/* option this */
	workspace_window_maximise( server->window );
	
	/* this could also be an option */
	b_window_focus( server );
	
	return server;
}


