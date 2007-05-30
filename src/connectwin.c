/*************************************************************************
$Id: connectwin.c 134 2005-08-06 00:55:06Z terminal $

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

CWidget *ctwin = 0;
int ctwintype = 0;

extern CWidget *mainwin;

extern XMLFile *servers;

// server
object_t *ct_lbl_servername, *ct_txt_servername;
object_t *ct_lbl_password, *ct_txt_password;
object_t *ct_lbl_port, *ct_txt_port;

// network
object_t *ct_lbl_network, *ct_ddn_network;
object_t *ct_lbl_server, *ct_ddn_server;

object_t *ct_cbx_newwin;
object_t *ct_btn_connect, *ct_btn_cancel;

XMLItem *ct_curr_network_sel;

void b_conto_killed( )
{
	ctwin = 0;
}

event_handler( b_ct_srv_txtchanged )
{
	char *server=0, *sport=0;
	int port;
	
	server = textbox_get_text(ct_txt_servername);
	sport = textbox_get_text(ct_txt_port);
	
	port = atoi( sport );
	
	if ( port == 0 || !strcasecmp( server, "" ) )
		widget_disable( OBJECT(ct_btn_connect) );
	else
		widget_enable( OBJECT(ct_btn_connect) );
}

event_handler( b_ct_connect )
{
	char *server=0, *sport=0;
	int port;
	char sbak[1024];
	BServerWindow *sw = 0;
	int newserv;
	XMLItem *xserver;
	char *srvdesc;
	char *text;
	list_item_t *sel, *nsel;
	int a;
	char *net = "";
	
	if ( ctwintype == 0 )
	{
		server = textbox_get_text(ct_txt_servername);
		sport = textbox_get_text(ct_txt_port);
		
		if ( !strcmp( server, "" ) || !strcmp( sport, "" ) )
			return;
	}
	else
	{
		nsel = combo_get_selected( ct_ddn_network );
		sel = combo_get_selected( ct_ddn_server );
		
		if ( sel == 0 || nsel == 0 )
			return;

		text = sel->data[0];
		a = 0;
		
		for ( xserver = ct_curr_network_sel->child_head; xserver != 0; xserver = xserver->next )
		{
			srvdesc = c_xml_attrib_get( xserver, "description" );
			
			if ( srvdesc == 0 )
				continue;
			
			if ( !strcasecmp( srvdesc, text ) )
			{
				server = c_xml_attrib_get( xserver, "name" );
				sport = c_xml_attrib_get( xserver, "ports" );
				
				if ( server == 0 || sport == 0 )
					continue;
				
				net = nsel->data[0];
				
				/* FIXME: port should be figured out from sport..
				 * and really each combination should be tried if
				 * connection fails. */
				sport = "6667";
				
				a = 1;
				
				break;
			}
		}
		
		if ( a == 0 )
			return;
	}
	
	port = atoi( sport );
	
	// do we have an active server?
	sw = b_server_find_active( );
	
	strcpy( sbak, server );

	// new server status...	
	newserv = checkbox_checked(ct_cbx_newwin);
	
	// disable window
	widget_disable( ctwin );
	
	// do we want a new server window?
	if ( newserv == 1 || sw == 0 )
	{
		sw = b_new_server_window( 0 );
	}
	
	if ( ctwintype == 0 )
		strcpy( sw->password, textbox_get_text(ct_txt_password) );
	else
		strcpy( sw->password, "" );
	
	b_server_connect( sw, sbak, port, net );
	
	// close window
	widget_close( ctwin );
}

event_handler( b_ct_srv_keypressed )
{
	int key = widget_get_notify_key( object, event );
	char *server=0, *sport=0;
	int port;
	
	if ( key != cKeyReturn )
		return;
	
	server = textbox_get_text(ct_txt_servername);
	sport = textbox_get_text(ct_txt_port);
	
	port = atoi( sport );
	
	if ( port == 0 || !strcasecmp( server, "" ) )
		return;
	
	b_ct_connect( object, event );
}

event_handler( b_ct_cancel )
{
	widget_close( OBJECT(ctwin) );
}

void b_conto_load_nets( )
{
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
		combo_append_row( ct_ddn_network, netname );
	}
}

event_handler( b_ct_net_selected )
{
	char *text;
	XMLItem *netlist;
	XMLItem *network;
	XMLItem *server;
	char *srvdesc;
	char *netname;
	list_item_t *sel=0, *item;
	
	item = combo_get_selected( ct_ddn_network );
	
	if ( item == 0 )
		return;
	
	text = item->data[0];
	
	netlist = c_xml_get_path( servers, "Bersirc.networklist" );
	
	if ( netlist == 0 )
		return;
	
	for ( network = netlist->child_head; network != 0; network = network->next )
	{
		netname = c_xml_attrib_get( network, "name" );
		
		if ( netname == 0 )
			continue;
			
		if ( !strcasecmp( netname, text ) )
		{
			ct_curr_network_sel = network;
			
			combo_clear( ct_ddn_server );
			
			for ( server = network->child_head; server != 0; server = server->next )
			{
				srvdesc = c_xml_attrib_get( server, "description" );
				
				if ( srvdesc == 0 )
					continue;
				
				item = combo_append_row( ct_ddn_server, srvdesc );
				
				if ( sel == 0 )
					sel = item;
			}
			
			if ( sel != 0 )
				combo_select_item( ct_ddn_server, sel );
			
			widget_enable( OBJECT(ct_ddn_server) );
			//c_widget_disable( ct_btn_connect );
			return;
		}
	}
}

event_handler( b_ct_netsrv_selected )
{
	char *text;
	list_item_t *item;
	
	item = combo_get_selected( ct_ddn_server );
	
	if ( item == 0 )
		return;
	
	text = item->data[0];
	
	if ( !strcasecmp( text, "" ) )
		return;
	
	widget_enable( ct_btn_connect );
	return;
}

void b_open_conto( int type )
{
	int y, yi, w, h, mpos;
	
	w = 350;
	h = (type == 0 ? 155 : 125 );
	yi = 30;
	mpos = 120;
	
	if ( ctwin == 0 )
	{
		ct_lbl_servername = ct_txt_servername = 0;
		ct_lbl_password = ct_txt_password = 0;
		ct_lbl_port = ct_txt_port = 0;
		ct_lbl_network = ct_ddn_network = 0;
		ct_lbl_server = ct_ddn_server = 0;
		ct_cbx_newwin = 0;
		ct_btn_connect = ct_btn_cancel = 0;
		ct_curr_network_sel = 0;
		
		ctwintype = type;
		
		ctwin = window_widget_create( bersirc->mainwin, new_bounds( -1, -1, w, h ), cWindowModalDialog | cWindowCenterParent );
		window_set_icon( ctwin, b_icon( (type==0?"connect_inp":"connect") ) );
		window_set_title( ctwin, lang_phrase_quick( (type==0?"connect_to_srv":"connect_to_net") ) );
		object_addhandler( ctwin, "destroy", b_conto_killed );
		
		y = 10;
		
		if ( type == 0 )
		{
			ct_lbl_servername = label_widget_create_with_text( ctwin, new_bounds(10, y, mpos, -1), 0, lang_phrase_quick( "connectwin_servername" ) );
			ct_txt_servername = textbox_widget_create( ctwin, new_bounds(mpos, y, w-mpos-10, -1), 0 );
			widget_focus( ct_txt_servername );
			y += 30;
			
			ct_lbl_password = label_widget_create_with_text( ctwin, new_bounds(10, y, mpos, -1), 0, lang_phrase_quick( "connectwin_password" ) );
			ct_txt_password = textbox_widget_create( ctwin, new_bounds(mpos, y, 140, -1), cTextBoxTypePassword );
			y += 30;
			
			ct_lbl_port =label_widget_create_with_text( ctwin, new_bounds(10, y, mpos, -1), 0, lang_phrase_quick( "connectwin_port" ) );
			ct_txt_port = textbox_widget_create( ctwin, new_bounds(mpos, y, 80, -1), 0 );
			textbox_set_text( ct_txt_port, "6667" );
			y += 30;
			
			widget_set_notify( OBJECT(ct_txt_servername), cNotifyKey );
			object_addhandler( ct_txt_servername, "key_down", b_ct_srv_keypressed );
			
			object_addhandler( ct_txt_servername, "changed", b_ct_srv_txtchanged );
			object_addhandler( ct_txt_port, "changed", b_ct_srv_txtchanged );
		}
		else
		{
			ct_lbl_network = label_widget_create_with_text( ctwin, new_bounds(10, y, mpos, -1), 0, lang_phrase_quick( "connectwin_network" ) );
			ct_ddn_network = combo_widget_create( ctwin, new_bounds(mpos, y, w-mpos-10, -1), 0 );
			widget_focus( ct_ddn_network );
			y += 30;
			
			ct_lbl_server =label_widget_create_with_text( ctwin, new_bounds(10, y, mpos, -1), 0, lang_phrase_quick( "connectwin_server" ) );
			ct_ddn_server = combo_widget_create( ctwin, new_bounds(mpos, y, w-mpos-10, -1), 0 );
			widget_disable( ct_ddn_server );
			y += 30;
			
			object_addhandler( ct_ddn_network, "selected", b_ct_net_selected );
			object_addhandler( ct_ddn_server, "selected", b_ct_netsrv_selected );
		}
		
		ct_cbx_newwin = checkbox_widget_create_with_label( ctwin, new_bounds(10, y, w-40, -1), 0, lang_phrase_quick( "connectwin_newwin" ) );
		checkbox_set_checked( ct_cbx_newwin, 1 );
		y += 25;
		
		ct_btn_connect = c_new_pushbutton( ctwin, lang_phrase_quick( "connect" ), w-220, y, 100, -1, 0 );
		widget_disable( OBJECT(ct_btn_connect) );
		ct_btn_cancel = c_new_pushbutton( ctwin, lang_phrase_quick( "cancel" ), w-110, y, 100, -1, 0 );
		y += 30;
		
		object_addhandler( ct_btn_cancel, "pushed", b_ct_cancel );
		object_addhandler( ct_btn_connect, "pushed", b_ct_connect );
		
		object_addhandler( ctwin, "dialog_ok", b_ct_connect );
		
		if ( type == 1 )
			b_conto_load_nets( );

		window_show( ctwin );
	}
	else
	{
		widget_focus( OBJECT(ctwin) );
	}
}
