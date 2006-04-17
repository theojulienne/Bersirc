/*************************************************************************
$Id: plugins.h 69 2005-05-08 06:06:55Z terminal $

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

#ifndef _BERSIRC_PLUGINS_H
#define _BERSIRC_PLUGINS_H

#ifdef _WIN32
	#include <windows.h>
#endif

#ifdef BERSIRC_PLUGIN
	/*#include "btextview.h"
	#include "irc.h"
	#include "irc_cmd.h"
	#include "irc_msg.h"*/
	//#include "includes.h"
#endif

#ifdef _WIN32
	#define BPTYPE __stdcall
#else
	#define BPTYPE
#endif

typedef int ( BPTYPE * BPHANDLER )( void *plugin, int message, void *data );

typedef struct
{
	char name[256];
	char author[256];
	int version;
	
	BPHANDLER handler;
} BersircPluginInit;

typedef struct
{
	void *window;
	char *text;
	int colour;
} BersircPluginWrite;

typedef struct
{
	char command[256];
	
	int (*function)( void *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window );
	
	int flags;
} BersircPluginCommand;

typedef struct b_plugin_struct
{
	object_t object;
	
#ifdef ENV_WIN32
	HMODULE plugin;
#else
	void *plugin;
#endif
	
	BPHANDLER handler;
	
	BersircPluginInit init;
	
	struct b_plugin_struct *next;
	struct b_plugin_struct *prev;
} bplugin_t;

#define BersircPlugin bplugin_t

enum
{
	B_MSG_APP_NULL=0,
	B_MSG_APP_COMMAND_ADD,
	B_MSG_APP_COMMAND_DEL,
	B_MSG_APP_HOOK_ADD,
	B_MSG_APP_HOOK_DEL,
	B_MSG_APP_SERVER_GET,
	B_MSG_APP_SERVER_FIND,
	B_MSG_APP_WIN_ACTIVE,
	B_MSG_APP_RAW,
	B_MSG_APP_WRITE,
};

enum
{
	B_MSG_PLG_NULL=0,
	B_MSG_PLG_INIT,
	B_MSG_PLG_DESTROY,
	B_MSG_PLG_ABOUT,
};

int b_plugin_load( char *filename );
void b_plugins_load( );

void *b_active_window(  );
int b_window_printf( void *win, int colour, char *fmt, ... );

CLFEXP int b_plugin_message( BersircPlugin *plugin, int message, void *data );

#endif

//
