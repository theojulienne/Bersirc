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

object_t *opt_time_line_timestamp = 0;
object_t *opt_time_long_timestamp = 0;

B_OPTIONS_PAGE_CREATE( b_options_time_page )
{
	object_t *cat;
	int y = 0;
	int sx = 180;
	
	cat = c_new_groupbox( parent, title, px, py, pw, ph, 0 );
	
	c_new_label( cat, lang_phrase_quick( "opt_time_line_timestamp" ), 0, y, sx, -1, 0 );
	opt_time_line_timestamp = textbox_widget_create( cat, new_bounds(sx+10, y, pw-30-sx, -1), 0 );
	textbox_set_text( opt_time_line_timestamp, b_get_option_string( identity, "time", "opt_time_line_timestamp" ) );
	y += 30;
	
	c_new_label( cat, lang_phrase_quick( "opt_time_long_timestamp" ), 0, y, sx, -1, 0 );
	opt_time_long_timestamp = textbox_widget_create( cat, new_bounds(sx+10, y, pw-30-sx, -1), 0 );
	textbox_set_text( opt_time_long_timestamp, b_get_option_string( identity, "time", "opt_time_long_timestamp" ) );
	y += 30;
	
	return cat;
}

B_OPTIONS_PAGE_SAVE( b_options_time_save )
{
	b_set_option_string( identity, "time", "opt_time_line_timestamp", textbox_get_text(opt_time_line_timestamp) );
	b_set_option_string( identity, "time", "opt_time_long_timestamp", textbox_get_text(opt_time_long_timestamp) );
}

//
