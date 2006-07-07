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

bersirc_t *bersirc;

event_handler( bersirc_mainloop_it )
{
	c_socket_run( );
	
	b_servers_run( );
}

void bersirc_create( )
{
	bersirc = (bersirc_t *)object_create( 0, sizeof(bersirc_t), "bersirc.core" );
	
	object_addhandler( claro, "mainloop", bersirc_mainloop_it );
}

////

int itter = 0;

XMLFile *config;
XMLItem *xidentity;
char identity[1024];
extern int savedprofile;
char filepath[4096];

char b_icon_theme[64];

typedef struct
{
	char *name;
	image_t *icon;
} BersircIcon;

BersircIcon bicons[] = {
	{ "bersirc", 0 },
	{ "connect", 0 },
	{ "disconnect", 0 },
	{ "network_browser", 0 },
	{ "profile_man", 0 },
	{ "channel_window", 0 },
	{ "query_window", 0 },
	{ "status_window", 0 },
	{ "url_catcher", 0 },
	{ "casc_windows", 0 },
	{ "tile_windows", 0 },
	{ "options", 0 },
	{ "tile_vertical", 0 },
	{ "channels", 0 },
	{ "time", 0 },
	{ "quitirc", 0 },
	{ "closewin", 0 },
	{ "add_fav", 0 },
	{ "cycle", 0 },
	{ "channel_props", 0 },
	{ "connect_inp", 0 },
	{ "folder_closed", 0 },
	{ "bug_report", 0 },
	{ "website", 0 },
	{ "help", 0 },
	{ "tree_channels", 0 },
	{ "server", 0 },
	{ "queries", 0 },
	{ 0, 0 },
};

image_t *b_icon( char *name )
{
	int a;
	char tmp[256];
	
	for ( a = 0; bicons[a].name != 0; a++ )
	{
		if ( !strcasecmp( bicons[a].name, name ) )
		{
			if ( bicons[a].icon == 0 )
			{
				sprintf( tmp, "themes/%s/icons/%s.png", b_icon_theme, name );
				bicons[a].icon = (image_t *)image_load( bersirc->mainwin, tmp );
				
				if ( bicons[a].icon == 0 )
				{
					sprintf( tmp, "themes/%s/icons/%s.png", "default", name );
					bicons[a].icon = (image_t *)image_load( bersirc->mainwin, tmp );;
				}
			}
			
			return bicons[a].icon;
		}
	}
	
	return 0;
}

void b_insert_systray( );
void b_remove_systray( );

int mw_state_x, mw_state_y, mw_state_w, mw_state_h, mw_state_max, mw_state_min;

event_handler( state_change_minimized )
{
	mw_state_min = 1;
	/* PORTFIX:
	if ( b_get_option_bool( xidentity, "general", "opt_gen_min_to_systray" ) )
	{
		b_insert_systray( );
		window_hide( bersirc->mainwin );
	}
	*/
}

event_handler( state_change_maximized )
{
	mw_state_max = 1;
	mw_state_min = 0;
	
	if ( !b_get_option_bool( xidentity, "general", "opt_gen_show_in_systray" ) )
		b_remove_systray( );
}

event_handler( state_change_restored )
{
	mw_state_max = 0;
	mw_state_min = 0;
	
	if ( !b_get_option_bool( xidentity, "general", "opt_gen_show_in_systray" ) )
		b_remove_systray( );
}

event_handler( state_change_moved )
{
	widget_t *widget = WIDGET(object);
	
	if ( mw_state_max )
		return;
	
	if ( widget->size.x > 0 || widget->size.y > 0 )
	{
		mw_state_x = widget->size.x;
		mw_state_y = widget->size.y;
	}
}

event_handler( state_change_resized )
{
	widget_t *widget = WIDGET(object);
	
	if ( mw_state_max )
		return;
	
	if ( widget->size_ct.w > 0 || widget->size_ct.h > 0 )
	{
		mw_state_w = widget->size_ct.w;
		mw_state_h = widget->size_ct.h;
	}
}
event_handler( state_changed )
{
/* PORTHACK */
#if 0
	if ( e->id == C_EVENT_MINIMIZED )
	{
		mw_state_min = 1;
		if ( b_get_option_bool( xidentity, "general", "opt_gen_min_to_systray" ) )
		{
			b_insert_systray( );
			c_widget_hide( bersirc->mainwin );
		}
	}
	else if ( e->id == C_EVENT_MAXIMIZED )
	{
		mw_state_max = 1;
		mw_state_min = 0;
	}
	else if ( e->id == C_EVENT_RESTORED )
	{
		mw_state_max = 0;
		mw_state_min = 0;
	}
	else
	{
		if ( e->caller->info->x > 0 || e->caller->info->y > 0 )
		{
			mw_state_x = e->caller->info->x;
			mw_state_y = e->caller->info->y;
			mw_state_w = e->caller->info->width;
			mw_state_h = e->caller->info->height;
		}
	}
	
	if ( mw_state_min == 0 )
	{
		if ( !b_get_option_bool( xidentity, "general", "opt_gen_show_in_systray" ) )
			b_remove_systray( );
	}
#endif
}

object_t *bersirc_systray_icon = 0;
object_t *bersirc_systray_popup = 0;
image_t *bersirc_icon;

void b_restore_me( )
{
	int was_maxed = mw_state_max;
	
	// show the window
	window_show( bersirc->mainwin );
	
	if ( was_maxed )
		window_maximise( bersirc->mainwin );// maximize it again if it was maximised
	else
		window_restore( bersirc->mainwin ); // otherwise, restore it
	
	window_focus( bersirc->mainwin );
}

event_handler( b_systray_dblclicked )
{
	b_restore_me( );
}

event_handler( b_systray_rclicked )
{
	/* PORTFIX */
	/*
	int *pos = (int *)data;
	
	if ( bersirc_systray_popup != 0 )
		c_menubar_popup( bersirc_systray_popup, pos[0], pos[1] );
	*/
}

void b_insert_systray( )
{
	if ( bersirc_systray_icon != 0 )
		return; // don't add twice.
	
	if ( bersirc_systray_popup == 0 )
	{
		/* PORTFIX */
		/*
		bersirc_systray_popup = c_new_menubar( bersirc->mainwin, C_MENU_POPUP );
		
		b_menu_from_xml( xmenu, bersirc_systray_popup, "context-systray", 0 );
		*/
	}
	
	// create systray icon widget
	/* PORTFIX */
	/*
	bersirc_systray_icon = c_new_systrayicon( bersirc->mainwin, "Bersirc", bersirc_icon, 0 );
	c_new_event_handler( bersirc_systray_icon, C_EVENT_MOUSE_LEFT_DBLCLK, b_systray_dblclicked );
	c_new_event_handler( bersirc_systray_icon, C_EVENT_MOUSE_RIGHT_CLICK, b_systray_rclicked );
	*/
}

void b_remove_systray( )
{
	if ( bersirc_systray_icon == 0 )
		return; // don't delete if not shown.
	
	// create systray icon widget
	/* PORTFIX */
	//c_destroy_widget( bersirc_systray_icon, 1 );
	
	bersirc_systray_icon = 0;
}

void b_shutdown( )
{
	// Save state
	b_set_option_int( xidentity, "state", "main_x", mw_state_x );
	b_set_option_int( xidentity, "state", "main_y", mw_state_y );
	b_set_option_int( xidentity, "state", "main_width", mw_state_w );
	b_set_option_int( xidentity, "state", "main_height", mw_state_h );
	b_set_option_bool( xidentity, "state", "main_maximise", mw_state_max );
	b_set_option_bool( xidentity, "state", "main_minimise", mw_state_min );
	
	// Save preferences!
	c_xml_dump_file( config, filepath );
}

void b_cleanup_and_exit( object_t *o, event_t *e )
{
	claro_shutdown( );
}

XMLFile *xmenu;

event_handler( profile_main_finished )
{
	claro_shutdown( );
}

event_handler( treeview_handle_context )
{
	object_t *item = OBJECT(event_get_arg_ptr( event, 0 ));
	BServerWindow *sw = (BServerWindow *)item->appdata;
	int dx, dy;
	
	if ( sw )
	{
		widget_screen_offset( object, &dx, &dy );
		dx += event_get_arg_int( event, 1 );
		dy += event_get_arg_int( event, 2 );
		menu_popup( sw->conmenu.menu, dx, dy, cMenuPopupAtCursor );
	}
}

event_handler( treeview_handle_selected )
{
	object_t *item = OBJECT(event_get_arg_ptr( event, 0 ));
	BServerWindow *sw;
	
	if ( !item )
		return;
	
	sw = (BServerWindow *)item->appdata;
	
	if ( sw )
		b_window_focus( sw->window );
}

int main( int argc, char *argv[] )
{
	char *realname, *username;
	XMLItem *nicks;
	BServerWindow *sw;
	int x, y, w, h, f;
	int startup_max=0;
	
	// Move the CWD so we're in the path of the executable if we started elsewhere
	char path[1024];
	char *tmp;
	strcpy( path, argv[0] );
	tmp = strrchr( path, '/' );
	if ( tmp != NULL )
	{
		printf( "%s\n", path );
		*tmp = 0;
		printf( "%s\n", path );
		if ( strcmp( path, "" ) )
			chdir( path );
	}
	else
	{
		tmp = strrchr( path, '\\' );
		if ( tmp != NULL )
		{
			printf( "%s\n", path );
			*tmp = 0;
			printf( "%s\n", path );
			if ( strcmp( path, "" ) )
				chdir( path );
		}
	}
	
	// FIXME: User should be able to select the identity within the current user XML file ?
	strcpy( identity, "Default" );
	
	// Initialise Claro
	claro_base_init( );
	claro_graphics_init( );
//	log_fd_set_level( CL_DEBUG, stderr );
	clog( CL_INFO, "%s (%s) running using Claro!", APPTITLE, __FILE__ );
	
	bersirc_create( );
	
	// Sort out the user directory
	config_userdir_init( );
	
	// Create XML parser and load prefs.xml (or the default if prefs.xml doesn't exist)
	config = c_xml_create( );
	
	sprintf( filepath, "%s/prefs.xml", userdir );
	
	if ( c_xml_load_file( config, filepath ) == 0 )
		c_xml_load_file( config, "xml/user-default.xml" );
	
	// Dump the XML parser back to prefs.xml, now it WILL exist
	c_xml_dump_file( config, filepath );
		
	// FIXME: identity should be loaded based on name and default.
	xidentity = c_xml_get_path( config, "Bersirc.identities.identity" );
	
	if ( xidentity == 0 )
	{
		printf( "Could not find Bersirc.identities.identity in user XML file. Bailing out\n" );
		return 0;
	}
	
	// Load our theme set
	strcpy( b_icon_theme, b_get_option_string( xidentity, "general", "opt_gen_icon_theme" ) );
	
	// Load the Bersirc icon
	bersirc_icon = b_icon( "bersirc" ); //image_load( bersirc->mainwin, "themes/default/icons/bersirc.png" );
	
	// Load our primary language (backup is always EN-UK)
	lang_init( b_get_option_string( xidentity, "general", "opt_gen_language" ) );
	
	// find the username, realname and nicks settings
	username = c_xml_attrib_get( xidentity, "username" );
	realname = c_xml_attrib_get( xidentity, "realname" );
	nicks = c_xml_find_child( xidentity, "nicks" );
	
	// make sure they are set
	if ( username == 0 || realname == 0 || !strcmp( username, "" ) || !strcmp( realname, "" ) || nicks == 0 || nicks->child_head == 0 )
	{
		object_t *pw;
		
		// otherwise, open the profile window
		bersirc->mainwin = 0;
		
		pw = b_open_profile( );
		object_addhandler( pw, "destroy", profile_main_finished );
		claro_loop( );
		
		if ( savedprofile == 0 )
			return 0;
	}
	
	// Initialise the commands & messages
	b_init_commands( );
	b_init_messages( );
	
	// Add Bersirc's (socket and other) proccessing to the loop
	//c_add_mainloop( (int (*)())bersirc_mainloop_it );
	
	// Create the main Bersirc window and set it's icon
	// Use saved state data
	
	x = b_get_option_int( xidentity, "state", "main_x" );
	y = b_get_option_int( xidentity, "state", "main_y" );
	w = b_get_option_int( xidentity, "state", "main_width" );
	h = b_get_option_int( xidentity, "state", "main_height" );
	f = 0;
	
	mw_state_x = x;
	mw_state_y = y;
	mw_state_w = w;
	mw_state_h = h;
	startup_max = mw_state_max = b_get_option_bool( xidentity, "state", "main_maximise" );
	mw_state_min = b_get_option_bool( xidentity, "state", "main_minimise" );
	/*
	if ( mw_state_max == 1 )
		f = C_START_MAXIMISE;
	
	if ( mw_state_min == 1 )
		f = C_START_MINIMISE;
	*/
	bounds_t *b = new_bounds(x, y, w, h);
	bersirc->mainwin = window_widget_create( 0, b, f );
	window_set_title( bersirc->mainwin, APPTITLE );
	window_set_icon( bersirc->mainwin, bersirc_icon );
	
	// Set up event handlers
	object_addhandler( bersirc->mainwin, "destroy", b_cleanup_and_exit );
	object_addhandler( bersirc->mainwin, "minimized", state_change_minimized );
	object_addhandler( bersirc->mainwin, "maximized", state_change_maximized );
	object_addhandler( bersirc->mainwin, "restored", state_change_restored );
	object_addhandler( bersirc->mainwin, "moved", state_change_moved );
	object_addhandler( bersirc->mainwin, "content_resized", state_change_resized );
	
	/* PORTFIX */
	/*
	c_new_event_handler( bersirc->mainwin, C_EVENT_MINIMIZED, state_changed );
	c_new_event_handler( bersirc->mainwin, C_EVENT_MAXIMIZED, state_changed );
	c_new_event_handler( bersirc->mainwin, C_EVENT_RESTORED, state_changed );
	c_new_event_handler( bersirc->mainwin, C_EVENT_RESIZE, state_changed );
	c_new_event_handler( bersirc->mainwin, C_EVENT_MOVE, state_changed );
	*/
	
	// Give the window a sane minimum size
	c_window_minsize( bersirc->mainwin, 300, 200 ); /* PORTFIX */
	
	// Load the default menu XML file
	xmenu = c_xml_create( );
	c_xml_load_file( xmenu, "xml/menus-default.xml" );
	
	if ( b_get_option_bool( xidentity, "general", "opt_gen_show_in_systray" ) )
		b_insert_systray( );
	
	// Create and load the toolbar
	bersirc->toolbar = toolbar_widget_create( bersirc->mainwin, 0 );
	b_menu_from_xml( xmenu, bersirc->toolbar, "toolbar", 1 );
	
	// Create and load the menu
	bersirc->menu = menubar_widget_create( bersirc->mainwin, 0 );
	b_menu_from_xml( xmenu, bersirc->menu, "application", 0 );
	
	// Create the layout for the content
	layout_t *lt;
	char lttmp[64];
	int tb_size = 25;
	
	if ( b_get_option_bool( xidentity, "taskbar", "opt_taskbar_enabled" ) == 0 )
		tb_size = 0;
	
	sprintf( lttmp, "[_workspace][{%d}taskbar]", tb_size );
	lt = layout_create( bersirc->mainwin, lttmp, *b, 20, 20 );
	bersirc->layout = lt;
	
	// MOO
	bersirc->splitter = splitter_widget_create( bersirc->mainwin, lt_bounds(lt,"workspace"), cSplitterHorizontal );
	bersirc->treeview = treeview_widget_create( bersirc->splitter, NO_BOUNDS, 0 );
	object_addhandler( bersirc->treeview, "right_clicked", treeview_handle_context );
	object_addhandler( bersirc->treeview, "selected", treeview_handle_selected );
	/*
	object_t *ti, *tti, *tti2;
	ti = treeview_append_row( bersirc->treeview, 0, b_icon("server"), "irc.free2code.net" );
	//list_item_set_text_color( ti, 1, 0, 0, 1 );
	list_item_set_font_extra( ti, cFontWeightBold, cFontSlantNormal, cFontDecorationNormal );
	tti = treeview_append_row( bersirc->treeview, ti, b_icon("tree_channels"), "Channels" );
	//list_item_set_text_color( tti, 0, 0, 1, 1 );
	       treeview_append_row( bersirc->treeview, tti, b_icon("channel_window"), "#test" );
	       treeview_append_row( bersirc->treeview, tti, b_icon("channel_window"), "#test2" );
	       treeview_append_row( bersirc->treeview, tti, b_icon("channel_window"), "#lobby" );
	tti2 = treeview_append_row( bersirc->treeview, ti, b_icon("queries"), "Private Chats" );
	treeview_expand( bersirc->treeview, tti );
	       treeview_append_row( bersirc->treeview, tti2, b_icon("query_window"), "Theo" );
	       treeview_append_row( bersirc->treeview, tti2, b_icon("query_window"), "Terminal" );
	       treeview_append_row( bersirc->treeview, tti2, b_icon("query_window"), "Bersirc" );
	
	treeview_expand( bersirc->treeview, ti );
	treeview_expand( bersirc->treeview, tti );
	treeview_expand( bersirc->treeview, tti2 );
	*/
	splitter_set_info( bersirc->splitter, cSplitterFirst, 0, 200 );
		
	// Workspace
	bersirc->workspace = workspace_widget_create( bersirc->splitter, NO_BOUNDS, 0 );
	
	// Task bar
	b_create_taskbar( bersirc->mainwin );
	
	// Status bar
	bersirc->statusbar = statusbar_widget_create( bersirc->mainwin, 0 );
	
	// prepare...
	list_create( &bersirc->servers );
	
	// Create the first status window
	sw = b_new_server_window( 1 );
	b_window_focus( sw );
	
	// Load the plugins (status window needs to be open just incase there are errors)
	b_plugins_load( );
	
	// Init DCC GUI data
	b_dcc_gui_init( );
	
	// Run auto-connect
	b_autowin_run( sw );
	
	// show main window
	//window_show( bersirc->mainwin );
	
	// maximise?
	if ( startup_max )
		window_maximise( bersirc->mainwin );
	else
		window_show( bersirc->mainwin );
	
	// Go into the loop!
	claro_loop( );
	
	// save, etc
	b_shutdown( );
	
	return 0;
}
