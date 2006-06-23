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

object_t *autowin = 0;

extern XMLItem *xidentity;
extern char filepath[4096];
extern XMLFile *config;

object_t *autowin_servlist;

void b_autowin_run( BServerWindow *first )
{
	XMLItem *autoi, *autoc;
	BServerWindow *sw = first;
	int a;
	char *place = "";
	char *server = "";
	int iport = 6667;
	char *pass = "";
	char *enabledtxt = "";
	
	autoi = c_xml_find_child( xidentity, "auto-connect" );
	
	if ( autoi == 0 )
		return;
	
	a = 0;

	for ( autoc = autoi->child_head; autoc != 0; autoc = autoc->next, a++ )
	{
		enabledtxt = c_xml_attrib_get( autoc, "enabled" );
		
		if ( enabledtxt != 0 && !strcmp( enabledtxt, "no" ) )
			continue;
		
		server = c_xml_attrib_get( autoc, "name" );
		place = c_xml_attrib_get( autoc, "channels" );
		
		if ( a > 0 || sw == 0 )
			sw = b_new_server_window( 0 );
		
		// tell sw where to go after connect
		b_server_onconnect_goto( sw, place );
		
		// set password
		strcpy( sw->password, pass );
		
		// connect sw to server
		b_server_connect( sw, server, iport, "" );
	}
}

void b_autowin_gui_init( )
{
	XMLItem *autoi, *autoc;
	char *nametxt, *chantxt, *enabledtxt;
	
	autoi = c_xml_find_child( xidentity, "auto-connect" );
	
	if ( autoi == 0 )
		return;

	for ( autoc = autoi->child_head; autoc != 0; autoc = autoc->next )
	{
		int a = 1;
		
		enabledtxt = c_xml_attrib_get( autoc, "enabled" );
		nametxt = c_xml_attrib_get( autoc, "name" );
		chantxt = c_xml_attrib_get( autoc, "channels" );
		
		if ( enabledtxt != 0 && !strcmp( enabledtxt, "no" ) )
			a = 0;
		
		listview_append_row( autowin_servlist, a, nametxt, chantxt );
	}
}

void b_autowin_gui_save( )
{
	XMLItem *autoi, *autoc;
	list_item_t *row;
	int rows;
	int a;
	
	if ( xidentity == 0 )
	{
		printf( "Invalid identity!\n" );
		exit( 0 );
	}
	
	// remove all items
	autoi = c_xml_find_child( xidentity, "auto-connect" );
	if ( autoi != 0 )
	{
		c_xml_clean_from( autoi->child_head );
		autoi->child_head = autoi->child_curr = 0;
	}
	else
	{
		autoi = c_xml_add_child( xidentity, "auto-connect" );
	}
	
	rows = listview_get_rows( autowin_servlist );
	
	for ( a = 0; a < rows; a++ )
	{
		/* FIXME: list_widget_get_row isn't very efficient :( */
		row = list_widget_get_row( autowin_servlist, 0, a );
		
		autoc = c_xml_add_child( autoi, "server" );
		
		if ( row->data[0] != 0 )
		{
			int *ip = (int *)row->data[0];
			c_xml_attrib_set( autoc, "enabled", (*ip) ? "yes" : "no" );
		}
		
		if ( row->data[1] != 0 )
			c_xml_attrib_set( autoc, "name", (char *)row->data[1] );
		
		if ( row->data[2] != 0 )
			c_xml_attrib_set( autoc, "channels", (char *)row->data[2] );
	}
	
	c_xml_dump_file( config, filepath );
}

event_handler( b_autowin_closing )
{
	b_autowin_gui_save( );
}

event_handler( b_autowin_killed )
{
	autowin = 0;
	autowin_servlist = 0;
}

event_handler( b_autowin_close )
{
	widget_close( WIDGET(autowin) );
}

object_t *autoeditwin = 0;

event_handler( b_autoeditwin_killed )
{
	autoeditwin = 0;
	
	b_open_autowin( );
}

event_handler( b_autoeditwin_close )
{
	widget_close( WIDGET( autoeditwin ) );
}

object_t *aew_txt_server, *aew_txt_channels, *aew_cbx_enabled;
list_item_t *aew_edit_item;

event_handler( b_autoeditwin_save )
{
	int pos;

	if ( strcmp( textbox_get_text( aew_txt_server ), "" ) == 0 )
		return;
	
	if ( aew_edit_item != 0 )
	{
		pos = aew_edit_item->row;
		listview_remove_row( autowin_servlist, aew_edit_item );
	}
	else
	{
		pos = -1;
	}
	
	aew_edit_item = listview_insert_row( autowin_servlist, pos, checkbox_checked(aew_cbx_enabled), textbox_get_text(aew_txt_server), textbox_get_text(aew_txt_channels) );
	
	b_autoeditwin_close( 0, 0 );
}

object_t *aw_btnsave, *aw_btncancel;

event_handler( b_autoeditwin_txtchanged )
{
	if ( !strcmp( textbox_get_text(object), "" ) )
	{
		widget_disable( WIDGET( aw_btnsave ) );
		return;
	}
	
	widget_enable( WIDGET( aw_btnsave ) );
}

void create_autoeditwin( list_item_t *item )
{
	int w, h, btn_w;
	char *srv_title, *chan_title, *srv_text, *chn_text;
	int text_w, c;
	object_t *aew_lbl_server, *aew_lbl_channels;
	layout_t *lt;
	bounds_t *b;
	char lt_str[512];
	int enable = 1;
	
	aew_edit_item = item;
	
	w = 400;
	h = 150;
	btn_w = (w-20-(10*(2-1))) / 2;
	
	b = new_bounds( -1, -1, w, h );
	autoeditwin = window_widget_create( bersirc->mainwin, b, cWindowModalDialog | cWindowCenterParent );
	window_set_icon( autoeditwin, b_icon("bersirc") );
	window_set_title( autoeditwin, lang_phrase_quick( (item==0?"addserver_t":"editserver_t") ) );
	object_addhandler( autoeditwin, "destroy", b_autoeditwin_killed );
	
	// input
	
	srv_title = lang_phrase_quick( "serveraddy_inp" );
	chan_title = lang_phrase_quick( "channels_inp" );
	
	srv_text = "";
	chn_text = "";
	
	if ( item != 0 )
	{
		int *ip = item->data[0];
		enable = *ip;
		srv_text = item->data[1];
		chn_text = item->data[2];
	}
	
	text_w = widget_font_string_width( bersirc->mainwin, srv_title, strlen( srv_title ) );
	c = widget_font_string_width( bersirc->mainwin, chan_title, strlen( chan_title ) );
	if ( c > text_w )
		text_w = c;
	
	sprintf( lt_str, "[][{25}<|(%d)lblserver|<|txtserver|<][][{25}<|(%d)lblchannels|<|txtchannels|<]"
					 "[][{25}<|enabled|<][][{25}<|btnsave|<|btncancel|<]", text_w, text_w );
	lt = layout_create( autoeditwin, lt_str, *b, 10, 10 );
	
	aew_lbl_server = label_widget_create_with_text( autoeditwin, lt_bounds(lt,"lblserver"), 0, srv_title );
	aew_txt_server = textbox_widget_create( autoeditwin, lt_bounds(lt,"txtserver"), 0 );
	textbox_set_text( aew_txt_server, srv_text );
	
	aew_lbl_channels = label_widget_create_with_text( autoeditwin, lt_bounds(lt,"lblchannels"), 0, chan_title );
	aew_txt_channels = textbox_widget_create( autoeditwin, lt_bounds(lt,"txtchannels"), 0 );
	textbox_set_text( aew_txt_channels, chn_text );
	
	object_addhandler( autoeditwin, "dialog_ok", b_autoeditwin_save );
	object_addhandler( aew_txt_server, "changed", b_autoeditwin_txtchanged );
	
	widget_focus( aew_txt_server );
	
	aew_cbx_enabled = checkbox_widget_create_with_label( autoeditwin, lt_bounds(lt,"enabled"), 0, lang_phrase_quick( "autoitemenable" ) );
	checkbox_set_checked( aew_cbx_enabled, enable );
	
	// buttons
	
	aw_btnsave = button_widget_create_with_label( autoeditwin, lt_bounds(lt,"btnsave"), 0, lang_phrase_quick( "save" ) );
	aw_btncancel = button_widget_create_with_label( autoeditwin, lt_bounds(lt,"btncancel"), 0, lang_phrase_quick( "cancel" ) );
	
	object_addhandler( aw_btnsave, "pushed", b_autoeditwin_save );
	object_addhandler( aw_btncancel, "pushed", b_autoeditwin_close );
	
	if ( item == 0 )
		widget_disable( WIDGET( aw_btnsave ) );
	
	window_show( autoeditwin );
}

event_handler( b_autowin_add )
{
	//widget_close( WIDGET(autowin) );
	
	create_autoeditwin( 0 );
}

event_handler( b_autowin_edit )
{
	list_item_t *a = listview_get_selected( autowin_servlist );
	
	//widget_close( WIDGET( autowin ) );
	
	create_autoeditwin( a );
}

object_t *btnadd, *btnedit, *btndelete, *btncancel;

event_handler( b_autowin_delete )
{
	list_item_t *a = listview_get_selected( autowin_servlist );
	
	if ( a == 0 )
		return;
	
	listview_remove_row( autowin_servlist, a );
	
	widget_disable( WIDGET( btndelete ) );
	widget_disable( WIDGET( btnedit ) );
}

event_handler( b_autowin_serversel )
{
	list_item_t *a = listview_get_selected( autowin_servlist );
	
	if ( a == 0 )
	{
		widget_disable( WIDGET( btndelete ) );
		widget_disable( WIDGET( btnedit ) );
		return;
	}
	
	widget_enable( WIDGET( btndelete ) );
	widget_enable( WIDGET( btnedit ) );
}

void b_open_autowin( )
{
	int y, yi, w, h, mpos;
	int btn_w;
	
	w = 650;
	h = 200;
	yi = 30;
	mpos = 120;
	
	if ( autowin == 0 )
	{
		bounds_t *b = new_bounds( -1, -1, w, h );
		layout_t *lt;
		
		autowin = window_widget_create( bersirc->mainwin, b, cWindowModalDialog | cWindowCenterParent );
		window_set_icon( autowin, b_icon( "bersirc" ) );
		window_set_title( autowin, lang_phrase_quick( "autoconnectwin" ) );
		object_addhandler( autowin, "destroy", b_autowin_killed );
		object_addhandler( autowin, "closing", b_autowin_closing );
		
		lt = layout_create( autowin, "[][_<|list|<][][{25}<|btnadd|<|btnedit|<|btndelete|<|btncancel|<][]", *b, 10, 10 );
		
		y = 10;
		
		autowin_servlist = listview_widget_create( autowin, lt_bounds(lt,"list"), 3, 0,
				"", cListViewTypeCheckBox,
				lang_phrase_quick( "serveraddy" ), cListViewTypeText,
				lang_phrase_quick( "channels" ), cListViewTypeText
			 );
		
		btn_w = ((w-20-20)/4);
		
		btnadd = button_widget_create_with_label( autowin, lt_bounds(lt,"btnadd"), 0, lang_phrase_quick( "addserver" ) );
		btnedit = button_widget_create_with_label( autowin, lt_bounds(lt,"btnedit"), 0, lang_phrase_quick( "editserver" ) );
		btndelete = button_widget_create_with_label( autowin, lt_bounds(lt,"btndelete"), 0, lang_phrase_quick( "deleteserver" ) );
		btncancel = button_widget_create_with_label( autowin, lt_bounds(lt,"btncancel"), 0, lang_phrase_quick( "close" ) );
		
		// no edit or delete until selected
		widget_disable( WIDGET( btndelete ) );
		widget_disable( WIDGET( btnedit ) );
		
		object_addhandler( autowin_servlist, "selected", b_autowin_serversel );
		object_addhandler( btnadd, "pushed", b_autowin_add );
		object_addhandler( btnedit, "pushed", b_autowin_edit );
		object_addhandler( btndelete, "pushed", b_autowin_delete );
		object_addhandler( btncancel, "pushed", b_autowin_close );
		
		window_show( autowin );
		
		b_autowin_gui_init( );
	}
	else
	{
		widget_focus( WIDGET( autowin ) );
	}
}
