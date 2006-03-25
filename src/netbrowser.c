/*************************************************************************
$Id: netbrowser.c 96 2005-06-15 05:44:01Z terminal $

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

CWidget *nbrowserwin = 0;

extern CWidget *mainwin;
extern XMLItem *xidentity;
extern char filepath[4096];
extern XMLFile *config;

CWidget *nb_label_title;
CWidget *nb_network_groupbox, *nb_server_groupbox;
CWidget *nb_texts[10];
CWidget *nb_buttons[4];
CWidget *nb_connect, *nb_close;
CWidget *nb_networks, *nb_servers;

XMLFile *servers = 0;

void b_netbrowser_killed( )
{
	nbrowserwin = 0;
}

void b_nbnetwork_selected( CEvent *e, void *data )
{
#ifdef ENV_WIN32
	//MessageBox( 0, ((CListBoxWidgetInfo *)e->caller->info)->selected->text, 0, 0 );
#endif
}

void b_netbrowser_save( CEvent *e, void *data )
{
	/*
	XMLItem *nicks;
	XMLItem *nick;
	CListBoxItem *citem;
	
	if ( xidentity == 0 )
	{
		printf( "Invalid identity!\n" );
		exit( 0 );
	}
	
	c_xml_attrib_set( xidentity, "username", ((CTextBoxWidgetInfo *)texts[0]->info)->text );
	c_xml_attrib_set( xidentity, "realname", ((CTextBoxWidgetInfo *)texts[1]->info)->text );
	
	nicks = c_xml_find_child( xidentity, "nicks" );
	c_xml_clean_from( nicks->child_head );
	nicks->child_head = nicks->child_curr = 0;
	
	for ( citem = ((CListBoxWidgetInfo *)nicklist->info)->items; citem != 0; citem = citem->next )
	{
		nick = c_xml_add_child( nicks, "nick" );
		c_xml_attrib_set( nick, "name", citem->text );
	}
	
	c_xml_dump_file( config, filepath );
	
	//b_netbrowser_cancel( e, data );
	*/
}

void b_netbrowser_load( )
{
#if 0 /* PORTHACK */
	XMLItem *netlist;
	XMLItem *network;
	char *netname;
	
	if ( servers == 0 )
	{
		servers = c_xml_create( );
		
		if ( !( c_xml_load_file( servers, "xml/servers-default.xml" ) ) )
			return;
	}
	
	netlist = c_xml_get_path( servers, "Bersirc.networklist" );
	
	if ( netlist == 0 )
		return;
	
	for ( network = netlist->child_head; network != 0; network = network->next )
	{
		netname = c_xml_attrib_get( network, "name" );
		c_listbox_additem( nb_networks, netname );
	}
#endif
}

void b_open_netbrowser( )
{
	int w, h;
#if 0 /* PORTHACK */
	w = 500;
	h = 450;
	
	if ( nbrowserwin == 0 )
	{
		nbrowserwin = c_new_window( mainwin, lang_phrase_quick( "netbrowser" ), w, h, C_DIALOG | C_WINDOW_CENTER_TO_PARENT );
		c_window_set_icon( nbrowserwin, b_icon( "network_browser" ) );
		c_window_maxsize( nbrowserwin, w, h );
		c_window_minsize( nbrowserwin, w, h );
		c_new_event_handler( nbrowserwin, C_EVENT_DESTROY, b_netbrowser_killed );
		
		nb_networks = c_new_listbox( nbrowserwin, 10, 10, 150, h-20, 0 );
		c_new_event_handler( nb_networks, C_EVENT_ITEM_SELECTED, b_nbnetwork_selected );
		
		nb_server_groupbox = c_new_groupbox( nbrowserwin, "Network's Servers", 170, 0, w-170-10, (h-30)/2-10, 0 );
		nb_network_groupbox = c_new_groupbox( nbrowserwin, "Network Details", 170, h/2, w-170-10, (h-30)/2-10, 0 );
		
		b_netbrowser_load( );
	}
	else
	{
		c_widget_focus( nbrowserwin );
	}
#endif
}
