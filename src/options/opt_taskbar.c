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

object_t *opt_taskbar_enabled = 0;

B_OPTIONS_PAGE_CREATE( b_options_taskbar_page )
{
	object_t *cat;
	int y = 0;
	
	cat = frame_widget_create_with_label( parent, new_bounds(px, py, pw, ph), 0, title );
	
	// visual
	
	opt_taskbar_enabled = c_new_checkbox( cat, lang_phrase_quick( "opt_taskbar_enable" ), 0, y, pw, -1, 0 );
	c_checkbox_checked( opt_taskbar_enabled, b_get_option_bool( identity, "taskbar", "opt_taskbar_enabled" ) );
	y += 20; // FIXME: This should be found some other way!
	
	return cat;
}

B_OPTIONS_PAGE_SAVE( b_options_taskbar_save )
{
	b_set_option_bool( identity, "taskbar", "opt_taskbar_enabled", checkbox_checked(opt_taskbar_enabled) );
}

//
