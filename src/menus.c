/*************************************************************************
$Id: berscore.c 156 2005-08-11 23:32:54Z terminal $

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

void menu_setup_favorites( object_t *menu, list_item_t *fav, XMLItem *head );

void do_x_click( int *wintype, char *cmd )
{	
	if ( cmd == 0 )
		return;
	
	if ( wintype == 0 )
	{
		b_user_command( 0, cmd, 2 );
		return;
	}
	
	if ( *wintype == B_CMD_WINDOW_STATUS )
		b_user_command( (BServerWindow *)wintype, cmd, 0 );
	else if ( *wintype & B_CMD_WINDOW_CHAT )
		b_user_command( (BChatWindow *)wintype, cmd, 1 );
}

event_handler( menu_x_click )
{
	int *wintype = 0;//(int *)b_active_window( );
	widget_t *po = WIDGET(object->parent);
	
	// see if we can find which window the menu belongs to
	if ( ( wintype = (int *)b_find_server_by_widget( OBJECT(po) ) ) == NULL )
	{
		wintype = (int *)b_find_chat_by_widget( OBJECT(po) );
	}
	
	if ( wintype == 0 )
		wintype = (int *)b_active_window( );
	
	do_x_click( wintype, object->appdata );
}

event_handler( tool_x_click )
{
	int *wintype = (int *)b_active_window( );
	do_x_click( wintype, object->appdata );
}

void b_menu_from_xml_item( XMLItem *i, object_t *menu, list_item_t *parent, int type )
{
	XMLItem *curr;
	list_item_t *m_new;
	list_item_t *t_new;
	image_t *icon;
	char *icon_n;
	char *item_n;
	char *enable;
	char *command;
	char *init;
	XMLItem *favlist;
	
	for ( curr = i->child_head; curr != 0; curr = curr->next )
	{
		icon = 0;
		
		if ( ( item_n = c_xml_attrib_get( curr, "title" ) ) == 0 )
			continue; // can't do anything about this one
		
		if ( !strcasecmp( item_n, "-" ) )
		{
			// separator
			if ( type == 4 )
				menu_append_separator( menu, parent );
			else if ( type == 0 )
				menubar_append_separator( menu, parent );
			else
				toolbar_append_separator( menu );
			continue;
		}
		
		// icon?
		if ( ( icon_n = c_xml_attrib_get( curr, "icon" ) ) != 0 )
			icon = b_icon( icon_n );
		
		// title, is it a language element?
		if ( lang_str_overlap( item_n, "lang:" ) == 5 )
			item_n = lang_phrase_quick( item_n + 5 );
		
		if ( type == 0 || type == 4 )
		{
			if ( type == 4 )
				m_new = menu_append_item( menu, parent, icon, item_n );
			else
				m_new = menubar_append_item( menu, parent, icon, item_n );
			
			if ( ( enable = c_xml_attrib_get( curr, "enabled" ) ) != 0 )
			{
				// disabled?
				if ( !strcasecmp( enable, "false" ) )
				{
					if ( type == 4 )
						menu_disable_item( menu, m_new );
					else
						menubar_disable_item( menu, m_new );
				}
			}
			
			if ( ( command = c_xml_attrib_get( curr, "command" ) ) != 0 )
			{
				OBJECT(m_new)->appdata = command;
				object_addhandler( OBJECT(m_new), "pushed", menu_x_click );
			}
			
			b_menu_from_xml_item( curr, menu, m_new, type );
			
			if ( ( init = c_xml_attrib_get( curr, "init" ) ) != 0 )
			{
				if ( !strcasecmp( init, "menu_setup_favorites" ) )
				{
					favlist = c_xml_find_child( xidentity, "favorites" );
					menu_setup_favorites( menu, m_new, favlist );
				}
			}
		}
		else
		{
			t_new = toolbar_append_icon( menu, icon, 0, item_n );
			
			if ( ( command = c_xml_attrib_get( curr, "command" ) ) != 0 )
			{
				OBJECT(t_new)->appdata = command;
				object_addhandler( OBJECT(t_new), "pushed", tool_x_click );
			}
			// we always recurse with a menu, toolbars are not toolbars in sub items
			//b_menu_from_xml_item( curr, menu, m_new, 0 );
			// but toolbar menus are not coded yet, so leave this out for now...
		}
	}
}

void b_menu_from_xml( XMLFile *xfile, CWidget *menu, char *name, int type )
{
	XMLItem *bers, *curr;
	char *cname;
	
	bers = c_xml_get_path( xfile, "Bersirc" );
	
	if ( bers == 0 )
		return;
	
	for ( curr = bers->child_head; curr != 0; curr = curr->next )
	{
		if ( strcasecmp( curr->name, "menu" ) )
			continue; // not a <menu>
		
		if ( ( cname = c_xml_attrib_get( curr, "name" ) ) == 0 )
			continue; 
		
		if ( strcasecmp( name, cname ) )
			continue;
		
		// this is our menu
		b_menu_from_xml_item( curr, menu, 0, type );
		
		return;
	}
}


