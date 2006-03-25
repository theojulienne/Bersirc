/*************************************************************************
$Id: dccwin.c 152 2005-08-10 01:17:04Z terminal $

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

CWidget *dccwin = 0;

extern CWidget *mainwin;

CWidget *dccwin_dcclist;
ClaroTable *dccwin_table;

void b_dcc_gui_init( )
{
#if 0 /* PORTHACK */
	ClaroTableRow *row;
	
	dccwin_table = c_tbl_create( 9 );
	
	row = c_tbl_insert_row( dccwin_table, 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 0, "blah.exe", 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 1, "Terminal", 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 2, "Transferring", 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 3, "800kb/s", 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 4, "1:19", 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 5, "1,234kb", 0 );
	c_tbl_set_cell_data( dccwin_table, 0, 6, "10.1.1.3", 0 );
#endif
}

void b_dccwin_killed( )
{
	dccwin = 0;
	dccwin_dcclist = 0;
}

void b_open_dccwin( )
{
#if 0 /* PORTHACK */
	int y, yi, w, h, mpos;
	
	w = 650;
	h = 200;
	yi = 30;
	mpos = 120;
	
	if ( dccwin == 0 )
	{
		dccwin = c_new_window( mainwin, "DCC Window [FIXME: NEED TO LANGUAGE-XML THIS]", w, h, C_DIALOG | C_WINDOW_CENTER_TO_PARENT );
		//c_window_set_icon( dccwin, b_icon( (type==0?"connect_inp":"connect") ) );
		c_window_maxsize( dccwin, w, h );
		c_window_minsize( dccwin, w, h );
		c_new_event_handler( dccwin, C_EVENT_DESTROY, b_dccwin_killed );
		
		y = 10;
		
		dccwin_dcclist = c_new_listview( dccwin, 0, 0, w, h, C_BORDER_NONE );
		c_listview_attach_table( dccwin_dcclist, dccwin_table );
		
		c_listview_add_table_column( dccwin_dcclist, "Filename", 0, 0 );
		c_listview_add_table_column( dccwin_dcclist, "User", 1, 1 );
		c_listview_add_table_column( dccwin_dcclist, "Status", 2, 2 );
		c_listview_add_table_column( dccwin_dcclist, "Speed", 3, 3 );
		c_listview_add_table_column( dccwin_dcclist, "ETA", 4, 4 );
		c_listview_add_table_column( dccwin_dcclist, "Size", 5, 5 );
		c_listview_add_table_column( dccwin_dcclist, "IP Address", 6, 6 );
	}
	else
	{
		c_widget_focus( dccwin );
	}
#endif
}
