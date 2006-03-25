/*************************************************************************
$Id: urlwin.c 167 2005-08-20 00:04:08Z terminal $

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

CWidget *urlwin = 0;

extern CWidget *mainwin;
extern BTextView *btv_head;

CWidget *btncopy, *btnclose, *btngo;
CWidget *urllist;

void b_urlwin_killed( )
{
	urlwin = 0;
}

void b_urlwin_close( CEvent *e, void *data )
{
	c_widget_close( urlwin );
}

void b_urlwin_populate( )
{
#if 0 /* PORTHACK */
	ClaroTable *table = ((CListBoxWidgetInfo *)urllist->info)->table;
	ClaroTableRow *row;
	ClaroTableCell *cell;
	BTextView *btv;
	btv_linkarea *links;

	for( btv = btv_head; btv != NULL; btv = btv->next )
		for ( links = btv->links; links != NULL; links = links->next )
		{
			row = c_tbl_insert_row( table, table->rows );
			cell = c_tbl_get_cell( table, row->pos, 0 );
			c_tbl_set_cell_data( table, row->pos, 0, strdup( links->url ), 1 );
		}
#endif
}

void b_urlwin_go( )
{
#if 0 /* PORTHACK */
	ClaroTable *table = ((CListBoxWidgetInfo *)urllist->info)->table;
	ClaroTableCell *cell;
	int a = c_listbox_selected( urllist );;

	if ( a == -1 )
		return;

	cell = c_tbl_get_cell( table, a, 0 );

	if ( cell )
		c_open_browser( c_application, cell->data );
#endif
}

void b_urlwin_urllist_selected( CEvent *e, void *data )
{
#if 0 /* PORTHACK */
	if ( c_listbox_selected( urllist ) == -1 )
	{
		c_widget_disable( btncopy );
		c_widget_disable( btngo );
		return;
	}
	
	c_widget_enable( btncopy );
	c_widget_enable( btngo );
#endif
}

void b_urlwin_copy( )
{
#if 0 /* PORTHACK */
	ClaroTable *table = ((CListBoxWidgetInfo *)urllist->info)->table;
	ClaroTableCell *cell;
	int a = c_listbox_selected( urllist );

	if ( a == -1 )
		return;

	cell = c_tbl_get_cell( table, a, 0 );

	if ( cell && btv_head )
		c_clipboard_set_text( btv_head->w, cell->data );
#endif
}

void b_open_urlwin( )
{
#if 0 /* PORTHACK */
	int y, yi, w, h;
	ClaroTable *table;
	
	w = 490;
	h = 400;
	yi = 30;
	
	if ( urlwin == 0 )
	{
		urlwin = c_new_window( mainwin, lang_phrase_quick( "url_list" ), w, h, C_DIALOG | C_WINDOW_CENTER_TO_PARENT );
		c_window_set_icon( urlwin, b_icon( "url_catcher" ) );
		c_new_event_handler( urlwin, C_EVENT_DESTROY, b_urlwin_killed );
		
		y = 10;
		urllist = c_new_listbox( urlwin, 10, y, w-20, 100, 0 );
		table = c_tbl_create( 1 );
		c_listbox_attach_table( urllist, table, 0 );
		c_widget_focus( urllist );
		y += 120;
		btngo = c_new_pushbutton( urlwin, lang_phrase_quick( "go" ), 10, y, 150, -1, 0 );
		btncopy = c_new_pushbutton( urlwin, lang_phrase_quick( "copy" ), 10+150+10, y, 150, -1, 0 );
		btnclose = c_new_pushbutton( urlwin, lang_phrase_quick( "close" ), 170+150+10, y, 150, -1, 0 );
		c_widget_move_info( urlwin, 0, 0, w, y + yi ); // update height
		
		c_window_maxsize( urlwin, w, y + yi );
		c_window_minsize( urlwin, w, y + yi );
		
		c_new_event_handler( btnclose, C_EVENT_BUTTON_PRESS, b_urlwin_close );
		c_new_event_handler( btncopy, C_EVENT_BUTTON_PRESS, b_urlwin_copy );
		c_new_event_handler( btngo, C_EVENT_BUTTON_PRESS, b_urlwin_go );
		c_new_event_handler( urllist, C_EVENT_ITEM_SELECTED, b_urlwin_urllist_selected );

		b_urlwin_populate( );
		c_widget_disable( btncopy );
		c_widget_disable( btngo );
	}
	else
	{
		c_widget_focus( urlwin );
	}
#endif
}
