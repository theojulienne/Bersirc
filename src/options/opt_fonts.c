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

BOptionFontItem items[] = {
	{ "opt_font_server_buffer", "server-buffer", "", "", 0, 0 },
	{ "opt_font_server_input", "server-input", "", "", 0, 0 },
	{ "opt_font_chat_buffer", "chat-buffer", "", "", 0, 0 },
	{ "opt_font_chat_input", "chat-input", "", "", 0, 0 },
	{ "", "", "", "", 0, 0 },
};

void b_opt_font_update_display( int a )
{
	sprintf( items[a].display, "%s, %d", items[a].face, items[a].size );
}

int b_fontd_by_loc_from_xml( BFontDef *font, char *loc );

void b_opt_font_choose_click( CEvent *e, void *data )
{
#if 0 /* PORTHACK */
	int a, b;
	
	for ( a = 0; ; a++ )
	{
		if ( !strcmp( items[a].name, "" ) )
			return;
		
		if ( e->caller == items[a].button )
			break;
	}
	
	c_choosefont_set_font_face( opt_font_select, items[a].face );
	c_choosefont_set_font_size( opt_font_select, items[a].size );
	
	b = c_choosefont_display( opt_font_select );
	
	if ( b == 0 )
	{
		return;
	}
	
	strcpy( items[a].face, c_choosefont_get_font_face( opt_font_select ) );
	items[a].size = c_choosefont_get_font_size( opt_font_select );
	
	b_opt_font_update_display( a );
	c_text_settext( items[a].input, items[a].display );
#endif
}

B_OPTIONS_PAGE_CREATE( b_options_font_page )
{
	BFontDef fontd;
	object_t *cat;
	int y = 0;
	int sx = 150;
	int a;
	
	cat = c_new_groupbox( parent, title, px, py, pw, ph, 0 );

	//opt_font_select = c_new_choosefont( parent );

	for ( a = 0; ; a++ )
	{
		if ( !strcmp( items[a].name, "" ) )
			break;
		
		c_new_label( cat, lang_phrase_quick( items[a].name ), 0, y, sx, -1, 0 );
		b_fontd_by_loc_from_xml( &fontd, items[a].cfg );
		
		strcpy( items[a].face, fontd.face );
		items[a].size = fontd.size;
		
		b_opt_font_update_display( a );
		
		items[a].input = textbox_widget_create( cat, new_bounds(sx, y, pw-20-sx-110, -1), 0 );
		textbox_set_text( items[a].input, items[a].display );
		items[a].button = c_new_pushbutton( cat, lang_phrase_quick( "choose" ), pw-120, y, 100, -1, 0 );
		
		widget_disable( items[a].input );
		
		object_addhandler( items[a].button, "pushed", b_opt_font_choose_click );
		
		y += 30;
	}

	/*
	c_new_label( cat, "Server buffer :", 10, y, sx, -1, 0 );
	b_fontd_by_loc_from_xml( &fontd, "server-buffer" );
	opt_font_server_buffer_font = fontd.face;
	opt_font_server_buffer_size = fontd.size;
	opt_font_server_buffer = c_new_text( cat, b_opt_font_get_str( opt_font_server_buffer_font, opt_font_server_buffer_size ), sx+10, y, pw-20-sx-110, -1, 0 );
	opt_font_server_buffer_btn = c_new_pushbutton( cat, lang_phrase_quick( "choose" ), pw-110, y, 100, -1, 0 );
	c_widget_disable( opt_font_server_buffer );
	y += 30;
	*/
	return cat;
}

extern BServerWindow *server_head;

void b_opt_fonts_set_font( XMLItem *p, int a )
{
#if 0 /* PORTHACK */
	XMLItem *fonts;
	char *xlocn;
	//char *size, *face;
	char tmp[32];
	BServerWindow *swin;
	BChatWindow *cwin;
	
	sprintf( tmp, "%d", items[a].size );
	
	for ( fonts = p->child_head; fonts; fonts = fonts->next )
	{
		if ( strcasecmp( fonts->name, "font" ) )
			continue;
		
		xlocn = 0;
		xlocn = c_xml_attrib_get( fonts, "location" );
		
		if ( xlocn == 0 )
			continue;
		
		if ( !strcasecmp( xlocn, items[a].cfg ) )
		{
			c_xml_attrib_set( fonts, "face", items[a].face );
			c_xml_attrib_set( fonts, "size", tmp );
						
			goto b_o_f_s_f_finish;
		}
	}
	
	fonts = c_xml_add_child( p, "font" );
	
	c_xml_attrib_set( fonts, "location", items[a].cfg );
	c_xml_attrib_set( fonts, "face", items[a].face );
	c_xml_attrib_set( fonts, "size", tmp );
	
	b_o_f_s_f_finish:
	
	for ( swin = server_head; swin != 0; swin = swin->next )
	{
		b_widget_set_font( swin->content->w, "server-buffer" );
		b_widget_set_font( swin->input, "server-input" );
		
		for ( cwin = swin->chat_head; cwin != 0; cwin = cwin->next )
		{
			b_widget_set_font( cwin->content->w, "chat-buffer" );
			b_widget_set_font( cwin->input, "chat-input" );
		}
	}
#endif
	return;
}

B_OPTIONS_PAGE_SAVE( b_options_font_save )
{
	XMLItem *i;
	int a;
#if 0 /* PORTHACK */
	i = c_xml_find_child( xidentity, "fonts" );
	
	if ( i == 0 )
	{
		i = c_xml_add_child( xidentity, "fonts" );
	}
	
	for ( a = 0; ; a++ )
	{
		if ( !strcmp( items[a].name, "" ) )
			break;
		
		b_opt_fonts_set_font( i, a );
	}
#endif
	//b_set_option_string( identity, "font", "opt_font_server_buffer", ((CTextBoxWidgetInfo *)opt_font_server_buffer->info)->text );
}

//
