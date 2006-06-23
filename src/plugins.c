/*************************************************************************
$Id: plugins.c 69 2005-05-08 06:06:55Z terminal $

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

#ifndef _WIN32
#include <dlfcn.h>
#endif

bplugin_t *plg_head = 0;

int b_window_printf( void *win, int colour, char *fmt, ... )
{
	int *wintype = win;
	va_list args;
	char buf[16384];
	
	if ( win == NULL )
		return 0;
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	if ( *wintype == B_CMD_WINDOW_STATUS )
		return b_swindow_printf( win, colour, "%s", buf );
	else
		return b_chatwin_printf( win, colour, "%s", buf );
}

void *b_active_window(  )
{
	CWidget *w;
	void *sw, *cw;
	
	w = workspace_get_active( bersirc->workspace );
	
	if ( w == 0 )
	{
		return 0;
	}
	
	sw = b_find_server_by_widget( w );
	
	if ( sw != 0 )
		return sw;
	
	cw = b_find_chat_by_widget( w );
	
	if ( cw != 0 )
		return cw;
	
	return 0;
}

int b_plugin_message( bplugin_t *plugin, int message, void *data )
{
	char **loc;
	BersircPluginWrite *bpw;
	BersircPluginCommand *bpc;
	
	switch ( message )
	{
		case B_MSG_APP_WIN_ACTIVE:
			loc = data;
			*loc = (char *)b_active_window( );
			break;
		case B_MSG_APP_WRITE:
			bpw = data;
			b_window_printf( bpw->window, bpw->colour, "%s", bpw->text );
			break;
		case B_MSG_APP_COMMAND_ADD:
			bpc = data;
			b_register_command( bpc->command, bpc->function, bpc->flags );
			break;
		default:
			return 0;
	}
	
	return 1;
}

int b_plugin_send( bplugin_t *plugin, int message, void *data )
{
	return (*plugin->handler)( plugin, message, data );
}

int b_plugin_load( char *filename )
{
	bplugin_t plg, *ptmp;
	
	memset( &plg, 0, sizeof( bplugin_t ) );
	
#ifdef _WIN32
	plg.plugin = LoadLibrary( filename );
#else
	plg.plugin = dlopen( filename, RTLD_NOW );
#endif
	
	if ( plg.plugin == 0 )
	{
		fprintf( stderr, "Failed to load plugin: %s\n", filename );
#ifndef _WIN32
		fprintf( stderr, "Reason: %s\n", dlerror( ) );
#endif
		return 0;
	}
	
#ifdef _WIN32
	plg.handler = GetProcAddress( plg.plugin, "Message" );
#else
	plg.handler = dlsym( plg.plugin, "Message" );
#endif
	
	plg.init.handler = (BPHANDLER)&b_plugin_message;
	
	if ( plg.handler == 0 || b_plugin_send( &plg, B_MSG_PLG_INIT, &plg.init ) != 1 )
	{
		fprintf( stderr, "Plugin loaded but the Message function did not exist or the init message returned invalid results\n" );

#ifdef _WIN32
		FreeLibrary( plg.plugin );
#else
		dlclose( plg.plugin );
#endif
		return 0;
	}
	
	// all worked.. let's save this plugin.
	
	ptmp = (bplugin_t *)malloc( sizeof(bplugin_t) );
	
	memcpy( ptmp, &plg, sizeof(bplugin_t) );
	
	ptmp->next = plg_head;
	if ( plg_head != 0 )
		plg_head->prev = ptmp;
	plg_head = ptmp;
	
	return 1;
}

void b_plugins_load( )
{
	XMLItem *item;
	char tmp[1024];
	char *env = "so";
	
#ifdef _WIN32
	env = "dll";
#endif
#ifdef _MAC
	env = "dylib";
#endif
	
	item = c_xml_find_child( xidentity, "plugins" );
	
	if ( item == 0 )
		return;
	
	for ( item = item->child_head; item != 0; item = item->next )
	{
		if ( !strcasecmp( item->name, "plugin" ) )
		{
			sprintf( tmp, "plugins/%s.%s", c_xml_attrib_get( item, "name" ), env );
			
			b_plugin_load( tmp );
		}
	}
}

//
