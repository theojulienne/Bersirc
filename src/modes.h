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

#define B_MODE_HANDLER_PARMS bserver_t *server, BChatWindow *chanwin, char dir, char mode, char *channel, char *victim

#define bModeHasNoParams 0

#define bModeHasParamOnSet (1)
#define bModeHasParamOnUnset (1<<1)
#define bModeHasParamAlways (bModeHasParamOnSet | bModeHasParamOnUnset)

#define bModeRemember (1<<2)

#define bModeTypeList (1<<3)

#define bModeTypeSetting (1<<4)
#define bModeTypePower ((1<<5) | bModeHasParamAlways)

typedef struct
{
	char mode;
	int (*handler)( B_MODE_HANDLER_PARMS );
	
	int flags;
} channel_mode_handler_t;

typedef struct
{
	char mode;
	char *victim;
} channel_mode_t;

void b_server_add_cmode( bserver_t *server, char mode, int flags );
void b_server_clear_cmodes( bserver_t *server, int mask );
int b_channel_mode_handle( bserver_t *server, bchannel_t *cw, char dir, char mode, char *channel, char *victim );

void b_channel_mode_list_clean( bchannel_t *cw );
