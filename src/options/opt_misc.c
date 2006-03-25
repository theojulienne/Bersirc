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

#include "../includes.h"

object_t *opt_misc_quit_message = 0;
object_t *opt_misc_quit_in_query = 0;
object_t *opt_misc_nick_in_query = 0;
object_t *opt_misc_nick_update_query = 0;
object_t *opt_misc_hide_version = 0;

B_OPTIONS_PAGE_CREATE( b_options_misc_page )
{
	object_t *cat;
	int y = 0;
	//int sx = 150;
	
	cat = c_new_groupbox( parent, title, px, py, pw, ph, 0 );
	
	c_new_label( cat, lang_phrase_quick( "opt_misc_quit_message" ), 0, y, pw, -1, 0 );
	y += 20;
	opt_misc_quit_message = textbox_widget_create( cat, new_bounds(20, y, pw-40, -1), 0 );
	textbox_set_text( opt_misc_quit_message, b_get_option_string( identity, "general", "opt_misc_quit_message" ) );
	y += 30;
	
	opt_misc_quit_in_query = c_new_checkbox( cat, lang_phrase_quick( "opt_misc_quit_in_query" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_misc_quit_in_query, b_get_option_bool( identity, "general", "opt_misc_quit_in_query" ) );
	y += 20;
	
	opt_misc_nick_in_query = c_new_checkbox( cat, lang_phrase_quick( "opt_misc_nick_in_query" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_misc_nick_in_query, b_get_option_bool( identity, "general", "opt_misc_nick_in_query" ) );
	y += 20;
	
	opt_misc_nick_update_query = c_new_checkbox( cat, lang_phrase_quick( "opt_misc_nick_update_query" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_misc_nick_update_query, b_get_option_bool( identity, "general", "opt_misc_nick_update_query" ) );
	y += 20;

	opt_misc_hide_version = c_new_checkbox( cat, lang_phrase_quick( "opt_misc_hide_version" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_misc_hide_version, b_get_option_bool( identity, "general", "opt_misc_hide_version" ) );
	y += 20;
	
	return cat;
}

B_OPTIONS_PAGE_SAVE( b_options_misc_save )
{
	b_set_option_string( identity, "general", "opt_misc_quit_message", textbox_get_text(opt_misc_quit_message) );
	b_set_option_bool( identity, "general", "opt_misc_quit_in_query", checkbox_checked(opt_misc_quit_in_query) );
	b_set_option_bool( identity, "general", "opt_misc_nick_in_query", checkbox_checked(opt_misc_nick_in_query) );
	b_set_option_bool( identity, "general", "opt_misc_nick_update_query", checkbox_checked(opt_misc_nick_update_query) );
	b_set_option_bool( identity, "general", "opt_misc_hide_version", checkbox_checked(opt_misc_hide_version) );
}

//
