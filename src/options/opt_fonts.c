/*************************************************************************
$Id: opt_fonts.c 150 2005-08-10 01:03:47Z terminal $

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

#include "../includes.h"

object_t *opt_font_select = 0;

typedef struct
{
	char name[64];
	char cfg[64];
	
	char display[128];
	
	char face[64];
	int size;
	
	object_t *input;
	object_t *button;
} BOptionFontItem;

BOptionFontItem bof_items[] = {
	{ "opt_font_server_buffer", "server-buffer", "", "", 0, 0 },
	{ "opt_font_server_input", "server-input", "", "", 0, 0 },
	{ "opt_font_chat_buffer", "chat-buffer", "", "", 0, 0 },
	{ "opt_font_chat_input", "chat-input", "", "", 0, 0 },
	{ "", "", "", "", 0, 0 },
};

void b_opt_font_update_display( int a )
{
	sprintf( bof_items[a].display, "%s, %d", bof_items[a].face, bof_items[a].size );
}

int b_fontd_by_loc_from_xml( BFontDef *font, char *loc );

extern object_t *optionswin;

event_handler( b_opt_font_choose_response )
{
	font_t *f;
	int accept = event_get_int( event, "response" );
	int a = (int)object->appdata;
	
	if ( accept )
	{
		f = font_dialog_get_font( object );
		
		strcpy( bof_items[a].face, f->face );
		bof_items[a].size = f->size;
		
		b_opt_font_update_display( a );
		textbox_set_text( bof_items[a].input, bof_items[a].display );
	}
	
	widget_close( object );
}

event_handler( b_opt_font_choose_click )
{
	int a;
	
	for ( a = 0; ; a++ )
	{
		if ( !strcmp( bof_items[a].name, "" ) )
			return;
		
		if ( object == bof_items[a].button )
			break;
	}
	
	opt_font_select = font_dialog_widget_create( optionswin, 0 );
	opt_font_select->appdata = (void *)a;
	
	object_addhandler( opt_font_select, "response", b_opt_font_choose_response );
	
	font_dialog_set_font( opt_font_select, bof_items[a].face, bof_items[a].size, 0, 0, 0 );
	
	widget_show( opt_font_select );
}

B_OPTIONS_PAGE_CREATE( b_options_font_page )
{
	BFontDef fontd;
	object_t *cat;
	int y = 0;
	int sx = 150;
	int a;
	
	cat = c_new_groupbox( parent, title, px, py, pw, ph, 0 );

	for ( a = 0; ; a++ )
	{
		if ( !strcmp( bof_items[a].name, "" ) )
			break;
		
		c_new_label( cat, lang_phrase_quick( bof_items[a].name ), 0, y, sx, -1, 0 );
		b_fontd_by_loc_from_xml( &fontd, bof_items[a].cfg );
		
		strcpy( bof_items[a].face, fontd.face );
		bof_items[a].size = fontd.size;
		
		b_opt_font_update_display( a );
		
		bof_items[a].input = textbox_widget_create( cat, new_bounds(sx, y, pw-20-sx-110, -1), 0 );
		textbox_set_text( bof_items[a].input, bof_items[a].display );
		bof_items[a].button = c_new_pushbutton( cat, lang_phrase_quick( "choose" ), pw-120, y, 100, -1, 0 );
		
		widget_disable( bof_items[a].input );
		
		object_addhandler( bof_items[a].button, "pushed", b_opt_font_choose_click );
		
		y += 30;
	}
	
	return cat;
}

extern BServerWindow *server_head;

void b_opt_fonts_set_font( XMLItem *p, int a )
{
	XMLItem *fonts;
	char *xlocn;
	char tmp[32];
	BServerWindow *swin;
	BChatWindow *cwin;
	node_t *n, *cn;
	
	sprintf( tmp, "%d", bof_items[a].size );
	
	for ( fonts = p->child_head; fonts; fonts = fonts->next )
	{
		if ( strcasecmp( fonts->name, "font" ) )
			continue;
		
		xlocn = 0;
		xlocn = c_xml_attrib_get( fonts, "location" );
		
		if ( xlocn == 0 )
			continue;
		
		if ( !strcasecmp( xlocn, bof_items[a].cfg ) )
		{
			c_xml_attrib_set( fonts, "face", bof_items[a].face );
			c_xml_attrib_set( fonts, "size", tmp );
						
			goto update_fonts;
		}
	}
	
	fonts = c_xml_add_child( p, "font" );
	
	c_xml_attrib_set( fonts, "location", bof_items[a].cfg );
	c_xml_attrib_set( fonts, "face", bof_items[a].face );
	c_xml_attrib_set( fonts, "size", tmp );
	
	update_fonts:
	
	LIST_FOREACH( n, bersirc->servers.head )
	{
		swin = (BServerWindow *)n->data;
		
		b_widget_set_font( swin->content, "server-buffer" );
		b_widget_set_font( swin->input, "server-input" );
		canvas_redraw( swin->content );
		
		LIST_FOREACH( cn, swin->chat_windows.head )
		{
			cwin = (BChatWindow *)cn->data;
			
			b_widget_set_font( cwin->content, "chat-buffer" );
			b_widget_set_font( cwin->input, "chat-input" );
			canvas_redraw( cwin->content );
		}
	}

	return;
}

B_OPTIONS_PAGE_SAVE( b_options_font_save )
{
	XMLItem *i;
	int a;

	i = c_xml_find_child( xidentity, "fonts" );
	
	if ( i == 0 )
	{
		i = c_xml_add_child( xidentity, "fonts" );
	}
	
	for ( a = 0; ; a++ )
	{
		if ( !strcmp( bof_items[a].name, "" ) )
			break;
		
		b_opt_fonts_set_font( i, a );
	}

	//b_set_option_string( identity, "font", "opt_font_server_buffer", ((CTextBoxWidgetInfo *)opt_font_server_buffer->info)->text );
}

//
