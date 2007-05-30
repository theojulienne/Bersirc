/*************************************************************************
$Id: profile.c 151 2005-08-10 01:10:11Z terminal $

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

object_t *profilewin = 0;

extern XMLItem *xidentity;
extern char filepath[4096];
extern XMLFile *config;

int savedprofile = 0;
int cansaveprof = 0;

object_t *labels[10];
object_t *texts[10];
object_t *buttons[4];
object_t *saveident, *btncancel;
object_t *nicklist;

void b_profile_killed( )
{
	profilewin = 0;
}

void b_profile_cansave_check( )
{
	char *t1t = textbox_get_text(texts[0]);
	char *t2t = textbox_get_text(texts[1]);
	
	int a = listbox_get_rows( nicklist );
	
	if ( !strcmp( t1t, "" ) || !strcmp( t2t, "" ) || a == 0 )
	{
		widget_disable( saveident );
		cansaveprof = 0;
		return;
	}
	
	widget_enable( saveident );
	cansaveprof = 1;
}

event_handler( b_profile_input_changed )
{
	b_profile_cansave_check( );
}

event_handler( b_profile_add_click )
{
	char *nick = textbox_get_text( texts[2] );
	
	if ( !strcmp( nick, "" ) )
		return;
	
	event->handled = 1;
	
	listbox_append_row( nicklist, nick );
	
	textbox_set_text( texts[2], "" );
	
	b_profile_cansave_check( );
	
	widget_focus( texts[2] );
}

event_handler( b_profile_del_click )
{
	list_item_t *a;
	
	a = listbox_get_selected( nicklist );
	
	if ( a == 0 )
		return;
	
	listbox_remove_row( nicklist, a );
	
	widget_disable( buttons[1] );
	
	b_profile_cansave_check( );
	
	widget_focus( nicklist );
}

event_handler( b_profile_cancel )
{
	widget_close( profilewin );
}

event_handler( b_profile_save )
{
	XMLItem *nicks;
	XMLItem *nick;
	int a, r;
	list_item_t *row;
	
	if ( cansaveprof == 0 )
		return;
	
	if ( xidentity == 0 )
	{
		printf( "Invalid identity!\n" );
		exit( 0 );
	}
	
	c_xml_attrib_set( xidentity, "username", textbox_get_text(texts[0]) );
	c_xml_attrib_set( xidentity, "realname", textbox_get_text(texts[1]) );
	
	nicks = c_xml_find_child( xidentity, "nicks" );
	c_xml_clean_from( nicks->child_head );
	nicks->child_head = nicks->child_curr = 0;

	r = listbox_get_rows( nicklist );
	for ( a = 0; a < r; a++ )
	{
		/* FIXME: list_widget_get_row isn't very efficient :( */
		row = list_widget_get_row( nicklist, 0, a );
		
		nick = c_xml_add_child( nicks, "nick" );
		if ( row->data[0] != 0 )
			c_xml_attrib_set( nick, "name", (char *)row->data[0] );
	}
	
	c_xml_dump_file( config, filepath );
	
	widget_close( profilewin );
	
	savedprofile = 1;
}

void b_profile_load( )
{
	XMLItem *nicks;
	XMLItem *nick;
	char *nicktxt;
	char *tmp;
	
	tmp = c_xml_attrib_get( xidentity, "username" );
	if ( tmp != 0 )
		textbox_set_text( texts[0], tmp );
	
	tmp = c_xml_attrib_get( xidentity, "realname" );
	if ( tmp != 0 )
		textbox_set_text( texts[1], tmp );
	
	nicks = c_xml_find_child( xidentity, "nicks" );
	
	for ( nick = nicks->child_head; nick != 0; nick = nick->next )
	{
		nicktxt = c_xml_attrib_get( nick, "name" );
		
		listbox_append_row( nicklist, nicktxt );
	}
	
	b_profile_cansave_check( );
}

event_handler( b_profile_nick_changed )
{
	if ( !strcmp( textbox_get_text(object), "" ) )
	{
		widget_disable( buttons[0] );
		return;
	}
	
	widget_enable( buttons[0] );
}

event_handler( b_profile_nicklist_selected );

event_handler( b_profile_up_click )
{
	list_item_t *a;
	
	a = listbox_get_selected( nicklist );
	
	if ( a == 0 )
		return;
	
	listbox_move_row( nicklist, a, a->row - 1 );
	listbox_select_item( nicklist, a );
}

event_handler( b_profile_down_click )
{
	list_item_t *a;
	
	a = listbox_get_selected( nicklist );
	
	if ( a == 0 )
		return;
	
	listbox_move_row( nicklist, a, a->row + 1 );
	listbox_select_item( nicklist, a );
}

event_handler( b_profile_nicklist_selected )
{
	list_item_t *item;
	int b, c;
	
	item = listbox_get_selected( nicklist );
	
	if ( item == 0 )
	{
		widget_disable( buttons[1] );
		widget_disable( buttons[2] );
		widget_disable( buttons[3] );
		return;
	}
	
	widget_enable( buttons[1] );
	
	b = item->row;
	c = listbox_get_rows( nicklist );
	
	if ( b > 0 )
		widget_enable( buttons[2] );
	else
		widget_disable( buttons[2] );
	
	if ( b < c-1 )
		widget_enable( buttons[3] );
	else
		widget_disable( buttons[3] );
}

event_handler( b_profile_key_down )
{
	int key = widget_get_notify_key( object, event );
	
	if ( key == cKeyReturn )
	{
		event->handled = 1;
		event_send( object, "enter_press", "" );
	}
}

object_t *b_open_profile( )
{
	int yi, w, h;
	layout_t *lt;
	char lt_str[512];
	int a, c, text_w;
	char *strs[3];
	
	w = 500;
	h = 300;
	yi = 30;
	
	if ( profilewin == 0 )
	{
		bounds_t *b = new_bounds( -1, -1, w, h );
		profilewin = window_widget_create( bersirc->mainwin, b, cWindowModalDialog | cWindowCenterParent );
		window_set_icon( profilewin, b_icon( "profile_man" ) );
		window_set_title( profilewin, lang_phrase_quick( "profile_man" ) );
		object_addhandler( profilewin, "destroy", b_profile_killed );
		
		strs[0] = lang_phrase_quick( "profilewin_username" );
		strs[1] = lang_phrase_quick( "profilewin_realname" );
		strs[2] = lang_phrase_quick( "profilewin_nickname" );
		
		text_w = 0;
		for ( a = 0; a < 3; a++ )
		{
			c = widget_font_string_width( profilewin, strs[a], strlen( strs[a] ) );
			if ( c > text_w )
				text_w = c;
		}
		
		if ( text_w < 100 )
			text_w = 100;
		
		sprintf( lt_str, "[]"
						 "[{25}<|(%d)lbluser|<|txtuser|<]"
						 "[{5}]"
						 "[{25}<|(%d)lblreal|<|txtreal|<]"
						 "[{5}]"
						 "[{25}<|(%d)lblnick|<|txtnick|<|(45)add|<|(45)del|<|(45)up|<|(45)down|<]"
						 "[]"
						 "[_<|list|<]"
						 "[]"
						 "[{25}<|btnsave|<|btncancel|<]"
						 "[]", text_w, text_w, text_w );
		lt = layout_create( profilewin, lt_str, *b, 10, 10 );
		
		labels[0] = label_widget_create_with_text( profilewin, lt_bounds(lt,"lbluser"), 0, strs[0] );
		labels[1] = label_widget_create_with_text( profilewin, lt_bounds(lt,"lblreal"), 0, strs[1] );
		labels[2] = label_widget_create_with_text( profilewin, lt_bounds(lt,"lblnick"), 0, strs[2] );
		
		texts[0] = textbox_widget_create( profilewin, lt_bounds(lt,"txtuser"), 0 );
		texts[1] = textbox_widget_create( profilewin, lt_bounds(lt,"txtreal"), 0 );
		texts[2] = textbox_widget_create( profilewin, lt_bounds(lt,"txtnick"), 0 );
		
		textbox_set_text( texts[0], "bersirc" );
		textbox_set_text( texts[1], "Bersirc User" );
		
		buttons[0] = button_widget_create_with_label( profilewin, lt_bounds(lt,"add"), 0, lang_phrase_quick( "profilewin_btn_add" ) );
		buttons[1] = button_widget_create_with_label( profilewin, lt_bounds(lt,"del"), 0, lang_phrase_quick( "profilewin_btn_del" ) );
		buttons[2] = button_widget_create_with_label( profilewin, lt_bounds(lt,"up"), 0, lang_phrase_quick( "profilewin_btn_up" ) );
		buttons[3] = button_widget_create_with_label( profilewin, lt_bounds(lt,"down"), 0, lang_phrase_quick( "profilewin_btn_down" ) );
		
		widget_disable( buttons[0] );
		widget_disable( buttons[1] );
		widget_disable( buttons[2] );
		widget_disable( buttons[3] );
		
		nicklist = listbox_widget_create( profilewin, lt_bounds(lt,"list"), 0 );
		
		saveident = button_widget_create_with_label( profilewin, lt_bounds(lt,"btnsave"), 0, lang_phrase_quick( "save_ident" ) );
		btncancel = button_widget_create_with_label( profilewin, lt_bounds(lt,"btncancel"), 0, lang_phrase_quick( "cancel" ) );
		
		widget_disable( saveident );
		
		object_addhandler( saveident, "pushed", b_profile_save );
		object_addhandler( btncancel, "pushed", b_profile_cancel );
		
		object_addhandler( buttons[0], "pushed", b_profile_add_click );
		object_addhandler( buttons[1], "pushed", b_profile_del_click );
		object_addhandler( buttons[2], "pushed", b_profile_up_click );
		object_addhandler( buttons[3], "pushed", b_profile_down_click );
		
		object_addhandler( profilewin, "dialog_ok", b_profile_save );
		
		object_addhandler( nicklist, "selected", b_profile_nicklist_selected );
		
		object_addhandler( texts[0], "changed", b_profile_input_changed );
		object_addhandler( texts[1], "changed", b_profile_input_changed );
		object_addhandler( texts[2], "changed", b_profile_nick_changed );
		
		widget_set_notify( OBJECT(texts[2]), cNotifyKey );
		object_addhandler( texts[2], "key_down", b_profile_key_down );
		object_addhandler( texts[2], "enter_press", b_profile_add_click );
		
		window_show( profilewin );
		
		b_profile_load( );
	}
	else
	{
		widget_focus( profilewin );
	}
	
	return profilewin;
}
