/*************************************************************************
$Id: opt_general.c 173 2005-09-19 01:45:00Z terminal $

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

object_t *opt_gen_open_new_query = 0;
object_t *opt_gen_rejoin_on_kick = 0;
object_t *opt_gen_show_ping_pong = 0;
object_t *opt_gen_flash_on_message = 0;
object_t *opt_gen_language = 0;
char    *opt_gen_lang_real = "en_uk";
object_t *opt_gen_show_in_systray = 0;
object_t *opt_gen_min_to_systray = 0;

XMLFile *xml_lang=0;

void b_options_general_load_languages( char *sel )
{
	XMLItem *langs;
	XMLItem *lang;
	char *langname, *langcode;
	list_item_t *i;
	
	if ( xml_lang == 0 )
	{
		xml_lang = c_xml_create( );
		
		if ( !( c_xml_load_file( xml_lang, "xml/lang.xml" ) ) )
			return;
	}
	
	langs = c_xml_get_path( xml_lang, "Bersirc.languages" );
	
	if ( langs == 0 )
		return;
	
	for ( lang = langs->child_head; lang != 0; lang = lang->next )
	{
		langname = c_xml_attrib_get( lang, "title" );
		langcode = c_xml_attrib_get( lang, "code" );
		
		if ( langcode == 0 || langname == 0 )
			continue;
		
		i = combo_append_row( opt_gen_language, langname );
		
		if ( !strcasecmp( langcode, sel ) )
			combo_select_item( opt_gen_language, i );
	}
}

event_handler( b_gen_language_selected )
{
	XMLItem *langs;
	XMLItem *lang;
	char *langname, *langcode;
	char *itemtxt = 0;
	list_item_t *i;
	
	i = combo_get_selected( object );
	
	if ( i == 0 )
		return;
	
	itemtxt = i->data[0];
	
	if ( itemtxt == 0 )
		return;
	
	langs = c_xml_get_path( xml_lang, "Bersirc.languages" );
	
	if ( langs == 0 )
		return;
	
	for ( lang = langs->child_head; lang != 0; lang = lang->next )
	{
		langname = c_xml_attrib_get( lang, "title" );
		langcode = c_xml_attrib_get( lang, "code" );
		
		if ( langcode == 0 || langname == 0 )
			continue;
		
		if ( !strcasecmp( langname, itemtxt ) )
		{
			opt_gen_lang_real = langcode;
		}
	}
}

B_OPTIONS_PAGE_CREATE( b_options_general_page )
{
	object_t *cat;
	int y = 0;
	//int sx = 200;
	
	cat = frame_widget_create( parent, new_bounds(px, py, pw, ph), title, 0 );
	
	// language
	
	int lang_w = 200;
	int text_w;
	
	char *l_title = lang_phrase_quick( "opt_gen_language" );
	
	text_w = widget_font_string_width( parent, l_title, strlen( l_title ) );
	text_w += 20;
	
	c_new_label( cat, l_title, 0, y, text_w, -1, 0 );
	opt_gen_language = combo_widget_create( cat, new_bounds( text_w, y, lang_w, 20 ), 0 );
	opt_gen_lang_real = b_get_option_string( identity, "general", "opt_gen_language" );
	b_options_general_load_languages( opt_gen_lang_real );
	object_addhandler( opt_gen_language, "selected", b_gen_language_selected );
	
	y += 30;
	
	// visual
	
	opt_gen_open_new_query = c_new_checkbox( cat, lang_phrase_quick( "opt_gen_open_new_query" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_gen_open_new_query, b_get_option_bool( identity, "general", "opt_gen_open_new_query" ) );
	y += 20; // FIXME: This should be found some other way!

	opt_gen_rejoin_on_kick = c_new_checkbox( cat, lang_phrase_quick( "opt_gen_rejoin_on_kick" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_gen_rejoin_on_kick, b_get_option_bool( identity, "general", "opt_gen_rejoin_on_kick" ) );
	y += 20;
	
	opt_gen_show_ping_pong = c_new_checkbox( cat, lang_phrase_quick( "opt_gen_show_ping_pong" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_gen_show_ping_pong, b_get_option_bool( identity, "general", "opt_gen_show_ping_pong" ) );
	y += 20;
	
	opt_gen_flash_on_message = c_new_checkbox( cat, lang_phrase_quick( "opt_gen_flash_on_message" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_gen_flash_on_message, b_get_option_bool( identity, "general", "opt_gen_flash_on_message" ) );
	y += 20;
	
	// systray
	
	opt_gen_show_in_systray = c_new_checkbox( cat, lang_phrase_quick( "opt_gen_show_in_systray" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_gen_show_in_systray, b_get_option_bool( identity, "general", "opt_gen_show_in_systray" ) );
	y += 20;
	
	opt_gen_min_to_systray = c_new_checkbox( cat, lang_phrase_quick( "opt_gen_min_to_systray" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_gen_min_to_systray, b_get_option_bool( identity, "general", "opt_gen_min_to_systray" ) );
	y += 20;
	
	return cat;
}

void b_insert_systray( );
void b_remove_systray( );

B_OPTIONS_PAGE_SAVE( b_options_general_save )
{
	char *tmp = strdup(opt_gen_lang_real);
	
	b_set_option_bool( identity, "general", "opt_gen_open_new_query", checkbox_checked(opt_gen_open_new_query) );
	b_set_option_bool( identity, "general", "opt_gen_show_ping_pong", checkbox_checked(opt_gen_show_ping_pong) );
	b_set_option_bool( identity, "general", "opt_gen_flash_on_message", checkbox_checked(opt_gen_flash_on_message) );
	b_set_option_bool( identity, "general", "opt_gen_rejoin_on_kick", checkbox_checked(opt_gen_rejoin_on_kick) );
	b_set_option_string( identity, "general", "opt_gen_language", tmp );
	
	free( tmp );
	
	// systray
	b_set_option_bool( identity, "general", "opt_gen_show_in_systray", checkbox_checked(opt_gen_show_in_systray) );
	if ( checkbox_checked(opt_gen_show_in_systray) )
		b_insert_systray( ); // make sure it's shown
	else
		b_remove_systray( );
	b_set_option_bool( identity, "general", "opt_gen_min_to_systray", checkbox_checked(opt_gen_min_to_systray) );
}

//
