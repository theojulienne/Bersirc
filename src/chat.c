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

BChatWindow *b_find_chat_by_widget( CWidget *w )
{
	BServerWindow *win;
	BChatWindow *cwin;
	node_t *n, *cn;
	
	LIST_FOREACH( n, bersirc->servers.head )
	{
		win = (BServerWindow *)n->data;
		
		LIST_FOREACH( cn, win->chat_windows.head )
		{
			cwin = (BChatWindow *)cn->data;
			
			if ( cwin->window == w || cwin->content == w || cwin->input == w || cwin->conmenu.menu == w )
			{
				return cwin;
			}
		}
	}
	
	return NULL;
}

BChatWindow *b_find_chat_by_dest( BServerWindow *win, char *dest )
{
	BChatWindow *cwin;
	node_t *n;

	LIST_FOREACH( n, win->chat_windows.head )
	{
		cwin = (BChatWindow *)n->data;
		if ( !strcasecmp( dest, cwin->dest ) )
		{
			return cwin;
		}
	}
	
	return NULL;
}

event_handler( b_chat_content_focus )
{
	BChatWindow *cw = b_find_chat_by_widget( object );
	
	if ( cw == 0 || cw->input == 0 )
		return;
	
	widget_focus( WIDGET(cw->input) );
	
	b_taskbar_redraw( );
}

event_handler( b_chat_enter_press )
{
	BChatWindow *win;
	
	event->handled = 1;
	
	if ( !strcmp( textbox_get_text(object), "" ) )
		return;
	
	if ( ( win = b_find_chat_by_widget( object->parent ) ) == NULL )
		return;
	
	//c_btv_addline( win->content, ((CTextBoxWidgetInfo *)obj->info)->text, 0, BTV_ClientWelcome );
	
	if ( b_user_command( win, textbox_get_text(object), 1 ) <= 0 )
		return;
	
	b_handle_enter_recall( object, (BServerWindow *)win );
	
	c_text_settext( object, "" );
	//c_send_event( win->content->w, C_EVENT_PREDRAW, 0 );
}

event_handler( b_chat_win_destroy )
{
	BChatWindow /**cwin, *cnext, */*deadwin;
	BServerWindow *server;
	
	deadwin = b_find_chat_by_widget( object );
	
	if ( deadwin == 0 )
		return;

	// find server
	server = (BServerWindow *)deadwin->server;
	
	// remove from list?
	if ( server != 0 )
	{
		node_t *n;
		n = node_find( deadwin, &server->chat_windows );
		node_del( n, &server->chat_windows );
		node_free( n );
		
		// remove the item first.
		treeview_remove_row( bersirc->treeview, deadwin->tv_item );
		
		if ( treeview_get_rows( bersirc->treeview, server->tv_channels ) == 0 )
		{
			treeview_remove_row( bersirc->treeview, server->tv_channels );
			server->tv_channels = 0;
		}
		
		if ( treeview_get_rows( bersirc->treeview, server->tv_queries ) == 0 )
		{
			treeview_remove_row( bersirc->treeview, server->tv_queries );
			server->tv_queries = 0;
		}
	}
	
	// finally, get rid of the structure
	free( deadwin );
	
	b_taskbar_redraw( );
}

void b_chat_update_title( BChatWindow *cw )
{
	/* PORTHACK
	char buf[1024];
	int users = 0;
	char modes[256];
	char modev[256];
	char tmp[2];
	int a;
	ClaroTableCell *cell;
	
	strcpy( buf, cw->dest );
	
	if ( cw->type == B_CMD_WINDOW_CHANNEL )
	{
		users = cw->users_table->rows;
		
		strcpy( modes, "" );
		strcpy( modev, "" );
		
		for ( a = 0; a < cw->channel_modes->rows; a++ )
		{
			cell = c_tbl_get_cell( cw->channel_modes, a, 0 );
			strcat( modes, cell->data );
			
			cell = c_tbl_get_cell( cw->channel_modes, a, 1 );
			if ( cell->data != 0 && strcmp( cell->data, "" ) )
			{
				strcat( modev, " " );
				strcat( modev, cell->data );
			}
		}
		
		sprintf( buf, "%s [%d] [+%s%s]: %s", cw->dest, users, modes, modev, cw->topic );
	}
	*/
	char buf[1024];
	strcpy( buf, cw->dest );
	workspace_window_set_title( cw->window, buf );
	b_taskbar_redraw( );
}

void b_userstore_updated( BChatWindow *win, BUserStore *user, int relocate )
{
	char *data;
	//int newpos;
	
	data = malloc( 1 + strlen( user->nickname ) + 1 );
		
	strcpy( data, "" );
		
	if ( b_find_user_prefix( user ) != 0 )
		sprintf( data, "%c", b_find_user_prefix( user ) );
		
	strcat( data, user->nickname );
	
	/* PORTHACK
	c_tbl_set_cell_data( win->users_table, user->row->pos, 0, data, 1 );
	
	if ( relocate == 1 )
	{
		newpos = b_find_table_row_position( win->users_table, user );
		c_tbl_move_row( win->users_table, user->row->pos, newpos );
	}
	*/
}

void b_chat_user_add( BChatWindow *win, BUserStore *user )
{
	BUserStore *nuser;
	ClaroTableRow *row;
	//ClaroTableCell *cell;
	
	if ( !( nuser = (BUserStore *)malloc( sizeof(BUserStore) ) ) )
		return;
	
	memcpy( nuser, user, sizeof(BUserStore) );
	
	nuser->next = nuser->prev = 0;
	
	if ( win->users == 0 )
	{
		win->users = nuser;
	}
	else
	{
		win->users->prev = nuser;
		nuser->next = win->users;
		win->users = nuser;
	}
	
	// add to users table
	
	/* PORTHACK
	row = c_tbl_insert_row( win->users_table, b_find_table_row_position( win->users_table, nuser ) );
	//cell = c_tbl_get_cell( win->users_table, row->pos, 0 );
	*/
	nuser->row = row;
	b_userstore_updated( win, nuser, 0 );
}

void b_chat_user_del( BChatWindow *win, char *nick )
{
	BUserStore *curr;
	
	for ( curr = win->users; curr != 0; curr = curr->next )
	{
		if ( !strcasecmp( nick, curr->nickname ) )
		{
			if ( curr == win->users )
			{
				win->users = curr->next;
			}
			else
			{
				curr->prev->next = curr->next;
			}
			
			if ( curr->next != 0 )
			{
				curr->next->prev = curr->prev;
			}
			
			// PORTHACK c_tbl_delete_row( win->users_table, curr->row->pos );
			
			free( curr );
			return;
		}
	}
}

void b_chat_user_empty( BChatWindow *win )
{
	BUserStore *curr, *next;
	
	for ( curr = win->users; curr != 0; curr = next )
	{
		next = curr->next;
		
		free( curr );
	}
	
	win->users = 0;
	
	// PORTHACK c_tbl_empty( win->users_table );
}

BUserStore *b_chat_user_find_nick( BChatWindow *win, char *nick )
{
	BUserStore *curr;
	
	for ( curr = win->users; curr != 0; curr = curr->next )
	{
		if ( !strcasecmp( nick, curr->nickname ) )
		{
			return curr;
		}
	}
	
	return 0;
}

int b_chatwin_printf( BChatWindow *cwin, int colour, char *fmt, ... )
{
	va_list args;
	char buf[16384];
	int of;
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	c_btv_addline( cwin->content, buf, 0, colour );
	
	
	of = cwin->taskbar_flags;
	
	if ( colour == BTV_Message || colour == BTV_Action )
	{
		if ( b_get_option_bool( xidentity, "general", "opt_gen_flash_on_message" ) )
		{
			c_window_flash( mainwin, 0, b_get_option_int( xidentity, "general", "opt_gen_flash_times" ) );
		}
		
		cwin->taskbar_flags |= 4;
	}
	else
		cwin->taskbar_flags |= 2;
	
	if ( of != cwin->taskbar_flags )
		b_taskbar_redraw( );
	
	return 1;
}

BChatWindow *b_new_chat_window( BServerWindow *server, char *dest, int flags )
{
	BChatWindow *chat;
	object_t *line, *cparent;
	bounds_t *cbounds;
	char *icon;
	node_t *n;
	char lt_str[256];
	
	chat = (BChatWindow *)malloc( sizeof( BChatWindow ) );
	
	memset( chat, 0, sizeof( BChatWindow ) );
	
	n = node_create( );
	node_add( chat, n, &server->chat_windows );
	
	strncpy( chat->dest, dest, 256 );
	
	chat->server = server;
	
	chat->recall_newest = 0;
	chat->recall_shown = 0;
	chat->recall_mode = 0;
	
	chat->users_table = 0;
	
	bounds_t *b = new_bounds(-1,-1,600,400);
	chat->window = workspace_window_widget_create( bersirc->workspace, b, 0 );
	sprintf( lt_str, "[_content][{1}line][{20}input]" );
	chat->layout = layout_create( chat->window, lt_str, *b, 20, 20 );
	
	object_addhandler( chat->window, "destroy", b_chat_win_destroy );
	object_addhandler( chat->window, "focus", b_chat_content_focus );
	object_addhandler( chat->window, "minimized", b_window_minimised );
	object_addhandler( chat->window, "maximized", b_window_maximised );
	object_addhandler( chat->window, "restored", b_window_restored );
	
	cparent = chat->window;
	cbounds = lt_bounds( chat->layout, "content" );
	
	if ( flags & 1 )
	{
		icon = "channel_window";
		
		/* create a splitter */
		chat->splitter = splitter_widget_create( chat->window, lt_bounds( chat->layout, "content" ), 0 );
		
		cparent = chat->splitter;
		cbounds = NO_BOUNDS;
	}
	else
	{
		icon = "query_window";
		chat->userlist = 0;
		chat->type = B_CMD_WINDOW_QUERY;
	}
		
	workspace_window_set_icon( server->window, b_icon( icon ) );
	
	chat->container = container_widget_create( cparent, cbounds, 0 );
	sprintf( lt_str, "[_content|scrollbar(%d)]", scrollbar_get_sys_width( ) );
	chat->ct_layout = layout_create( chat->container, lt_str, *cbounds, 20, 20 );
	
	chat->content = (object_t *)ircview_widget_create( chat->container, lt_bounds(chat->ct_layout,"content") ); //cparent, cbounds );
	object_addhandler( chat->content, "focus", b_chat_content_focus );
	
	chat->scroll = scrollbar_widget_create( chat->container, lt_bounds(chat->ct_layout,"scrollbar"), cScrollbarVertical );
	ircview_set_scrollbar( chat->content, chat->scroll );
	
	if ( flags & 1 )
	{
		chat->userlist = listbox_widget_create( chat->splitter, NO_BOUNDS, cWidgetNoBorder );
		chat->type = B_CMD_WINDOW_CHANNEL;
		splitter_set_info( chat->splitter, cSplitterSecond, 0, 100 );
		
		/*chat->users_table = c_tbl_create( 2 );
		c_listbox_attach_table( chat->userlist, chat->users_table, 0 );
		
		chat->channel_modes = c_tbl_create( 2 );*/
	}
	
	line = canvas_widget_create( chat->window, lt_bounds(chat->layout,"line"), 0 );
	object_addhandler( line, "redraw", b_draw_line_canvas );
	
	chat->input = textbox_widget_create( chat->window, lt_bounds(chat->layout,"input"), cWidgetNoBorder );
	widget_set_notify( WIDGET(chat->input), cNotifyKey );
	
	object_addhandler( chat->input, "key_down", b_input_key_press );
	object_addhandler( chat->input, "enter_press", b_chat_enter_press );
	
	/* config fonts */
	b_widget_set_font( chat->content, "chat-buffer" );
	b_widget_set_font( chat->input, "chat-input" );
	
	// add to the treeview
	if ( flags & 1 )
	{
		int newf=0;
		
		// channel
		if ( server->tv_channels == 0 )
		{
			server->tv_channels = treeview_insert_row( bersirc->treeview, server->tv_item, 0, b_icon("tree_channels"), "Channels" ); // FIXME: language
			newf = 1;
		}
		
		chat->tv_item = treeview_append_row( bersirc->treeview, server->tv_channels, b_icon("channel_window"), chat->dest );
		chat->tv_item->appdata = (void *)chat;
		
		if ( newf )
		{
			treeview_expand( bersirc->treeview, server->tv_channels );
			treeview_expand( bersirc->treeview, server->tv_item );
		}
	}
	else
	{
		int newf=0;
		
		// query
		if ( server->tv_queries == 0 )
		{
			server->tv_queries = treeview_append_row( bersirc->treeview, server->tv_item, b_icon("queries"), "Private Chats" ); // FIXME: language
			newf = 1;
		}
		
		chat->tv_item = treeview_append_row( bersirc->treeview, server->tv_queries, b_icon("query_window"), chat->dest );
		chat->tv_item->appdata = (void *)chat;
		
		if ( newf )
		{
			treeview_expand( bersirc->treeview, server->tv_queries );
			treeview_expand( bersirc->treeview, server->tv_item );
		}
	}
	
	b_taskbar_redraw( );
	
	b_window_create_context_menu( (BServerWindow *)chat );
	
	b_chat_update_title( chat );
	
	workspace_window_show( chat->window );
	
	/* option this */
	workspace_window_maximise( chat->window );
	
	return chat;
}

