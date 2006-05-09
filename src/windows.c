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

extern XMLFile *xmenu;

BServerWindow *b_find_any_by_widget( object_t *w )
{
	BServerWindow *sw;
	
	// find server or chat, we'll only be using the shared parts
	if ( ( sw = b_find_server_by_widget( w ) ) == NULL )
	{
		if ( ( sw = (BServerWindow *)b_find_chat_by_widget( w ) ) == NULL )
			return 0;
	}
	
	return sw;
}

event_handler( b_window_minimised )
{
	BServerWindow *sw = b_find_any_by_widget( object );
	
	// we're minimised
	sw->window_flags |= B_WINDOW_MINIMISE;
	
	workspace_window_hide( object );
}

event_handler( b_window_maximised )
{
	BServerWindow *sw = b_find_any_by_widget( object );
	
	// we're maximised
	sw->window_flags |= B_WINDOW_MAXIMISE;
}

event_handler( b_window_restored )
{
	BServerWindow *sw = b_find_any_by_widget( object );
	
	// turn them both off
	sw->window_flags |= B_WINDOW_MINIMISE | B_WINDOW_MAXIMISE;
	sw->window_flags -= B_WINDOW_MINIMISE | B_WINDOW_MAXIMISE;
}

void b_window_focus( object_t *w )
{
	BServerWindow *sw = b_find_any_by_widget( w );
	
	if ( sw == 0 )
		sw = (BServerWindow *)w;
	
	c_widget_show( w );
	
	if ( sw->window_flags & B_WINDOW_MINIMISE )
	{
		// no longer minimised flag
		sw->window_flags -= B_WINDOW_MINIMISE;
		
		if ( sw->window_flags & B_WINDOW_MAXIMISE )
			workspace_window_maximize( w );
		else
			workspace_window_restore( w );
	}
	
	widget_focus( w );
}

void b_window_create_context_menu( BServerWindow *win )
{
	int type;
	
	type = win->type;
	
	win->conmenu.menu = menu_widget_create( win->window, 0 );
	
	if ( type & B_CMD_WINDOW_STATUS )
		b_menu_from_xml( xmenu, win->conmenu.menu, "context-status", 4 );
	else if ( type & B_CMD_WINDOW_CHANNEL )
		b_menu_from_xml( xmenu, win->conmenu.menu, "context-channel", 4 );
	else if ( type & B_CMD_WINDOW_QUERY )
		b_menu_from_xml( xmenu, win->conmenu.menu, "context-query", 4 );
}

char *b_window_autocomplete( bchannel_t *cw )
{
	int rnum = 0;
	bserver_t *sw;
	char *start = cw->last_tab_comp;
	int result = cw->tab_comp_num;
	node_t *n;
	BUserStore *curr;
	
	if ( cw->type == B_CMD_WINDOW_CHANNEL )
	{
		for ( curr = cw->users; curr != 0; curr = curr->next )
		{
			char *text = curr->nickname;
			if ( !strncasecmp( start, text, strlen( start ) ) )
			{
				rnum++;
				if ( rnum > result )
				{
					cw->tab_comp_num++;
					return text;
				}
			}
		}
	}
	
	sw = cw;
	
	if ( cw->type != B_CMD_WINDOW_STATUS )
		sw = cw->server;
	
	LIST_FOREACH( n, sw->chat_windows.head )
	{
		bchannel_t *nc = (bchannel_t *)n->data;
		char *text = nc->dest;
		
		if ( nc->type != B_CMD_WINDOW_CHANNEL )
			continue;
		
		if ( !strncmp( start, text, strlen( start ) ) )
		{
			rnum++;
			if ( rnum > result )
			{
				cw->tab_comp_num++;
				return text;
			}
		}
	}
	
	// did we find one or more but go past the end?
	// wasteful, but loop!
	if ( rnum > 0 )
	{
		cw->tab_comp_num = 0;
		return b_window_autocomplete( cw );
	}
	
	return 0;
}

event_handler( b_input_key_press )
{
	/* PORTHACK */
	int key = widget_get_notify_key( object, event );
	BServerWindow *win;
	BTextRecall *tmp, *rpl=0;
	ircview_t *content;
	
	if ( ( win = b_find_server_by_widget( object->parent ) ) == NULL )
	{
		if ( ( win = (BServerWindow *)b_find_chat_by_widget( object->parent ) ) == NULL )
			return;
		else
			content = (ircview_t *)((BChatWindow *)win)->content;
	} else
		content = (ircview_t *)win->content;
	
	if ( key == cKeyUp )
	{
		if ( win->recall_shown != 0 )
			rpl = win->recall_shown->next;
		else
			rpl = win->recall_newest;
	}
	else if ( key == cKeyDown )
	{
		if ( win->recall_shown != 0 )
			rpl = win->recall_shown->prev;
		else
			rpl = 0;
	}
	else if ( key == cKeyPageUp )
	{
		c_scroll_page( content->w, C_SCROLLING_VERT, -1 );
		return;
	}
	else if ( key == cKeyPageDown )
	{
		c_scroll_page( content->w, C_SCROLLING_VERT, 1 );
		return;
	}
	else if ( key == cKeyReturn )
	{
		event->handled = event_send( object, "enter_press", "" );
		goto clear_tab_comp;
		return;
	}
	else if ( key == cKeyTab )
	{
		char *text, *tmp, *res;
		char cpy[CLARO_TEXTBOX_MAXIMUM];
		int a;
		
		// FIXME: this should really be checking from where the cursor is, not just the end of the line.
		
		text = textbox_get_text( object );
		strcpy( cpy, text );
		text = &cpy;
		tmp = "";
		
		for ( a = strlen( text ); a >= 0; a-- )
		{
			tmp = text + a;
			
			if ( tmp[0] == ' ' )
			{
				tmp++;
				break;
			}
		}
		
		if ( !strcmp( text, "" ) )
			return;
		
		if ( win->last_tab_comp != 0 )
		{
			// again
			res = b_window_autocomplete( win );
			if ( res == 0 )
				return;
			strcpy( tmp, res );
			
			textbox_set_text( object, text );
			textbox_set_pos( object, strlen(text) );
			
			event->handled = 1;
			return;
		}
		
		win->last_tab_comp = strdup( tmp );
		win->tab_comp_num = 0;
		
		res = b_window_autocomplete( win );
		if ( res == 0 )
			return;
		strcpy( tmp, res );
		
		textbox_set_text( object, text );
		textbox_set_pos( object, strlen(text) );
		
		event->handled = 1;
		return;
	}
	else
	{
		clear_tab_comp:
		if ( win->last_tab_comp != 0 )
		{
			free( win->last_tab_comp );
			win->last_tab_comp = 0;
			win->tab_comp_num = 0;
		}
		return; // we don't care
	}
	
	event->handled = 1;
	
	// if the replacement was 0, there's nothing we can do!
	if ( rpl == 0 )
		return;
	
	// set the event to "handled", so the OS doesn't do things with the keypress
	//e->proc_cancel = 1;
	/* PORTHACK */
	
	// if we're not in recall mode, save this line as the newest TR record
	if ( win->recall_mode == 0 )
	{
		tmp = (BTextRecall *)malloc( sizeof( BTextRecall ) );
		
		tmp->input = strdup( textbox_get_text(object) );
		
		tmp->next = win->recall_newest;
		tmp->prev = 0;
		
		if ( win->recall_newest != 0 )
			win->recall_newest->prev = tmp;
		
		win->recall_newest = tmp;
		
		win->recall_mode = 1;
	}
	
	// replace input text with replacement
	c_text_settext( object, rpl->input );
	c_text_setsel( object, strlen( rpl->input ), strlen( rpl->input ) );
	
	// remember where we are
	win->recall_shown = rpl;
	
	if ( rpl == win->recall_newest )
	{
		// re've moved back to the beginning. exit recall mode!
		tmp = rpl->next;
		free( rpl->input );
		free( rpl );
		tmp->prev = 0;
		
		win->recall_newest = tmp;
		
		win->recall_mode = 0;
		win->recall_shown = 0;
	}
	
	/* clear tab completion here too */
	if ( win->last_tab_comp != 0 )
	{
		free( win->last_tab_comp );
		win->last_tab_comp = 0;
		win->tab_comp_num = 0;
	}
}

void b_handle_enter_recall( object_t *input, BServerWindow *win )
{
	BTextRecall *tr;
	
	if ( win->recall_mode == 1 )
	{
		// we are in recall mode, so re-use the saved input TR record instead
		tr = win->recall_newest;
		
		// free old text
		free( tr->input );
	}
	else
	{
		tr = (BTextRecall *)malloc( sizeof( BTextRecall ) );
		
		tr->next = win->recall_newest;
		tr->prev = 0;
		
		if ( win->recall_newest != 0 )
			win->recall_newest->prev = tr;
		
		win->recall_newest = tr;
	}
	
	// get out of recall mode
	win->recall_mode = 0;
	win->recall_shown = 0;
	
	tr->input = strdup( textbox_get_text(input) );
}

event_handler( b_draw_line_canvas )
{
	widget_t *w = (widget_t *)object;
	// FIXME: Colour should be configurable, same time as everything else is :)
	canvas_fill_rect( w, 0, 0, w->size_req->w, w->size_req->h, 0.83f, 0.81f, 0.78f, 1.0f );
}


