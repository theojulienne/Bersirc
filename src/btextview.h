/*************************************************************************
$Id: btextview.h 175 2005-10-17 02:54:21Z terminal $

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

#define BTV_ClientWelcome 0
#define BTV_ConnectionMsg 1
#define BTV_Error 2
#define BTV_PingPong 3
#define BTV_UserJoin 4
#define BTV_Info 5
#define BTV_UserQuit 6
#define BTV_UserNick 7
#define BTV_TopicChange 8
#define BTV_MOTD 9
#define BTV_UserPart 10
#define BTV_CTCP 11
#define BTV_Notice 12
#define BTV_Alert 13
#define BTV_Topic 14
#define BTV_UserKick 15
#define BTV_TopicTime 16
#define BTV_Debug 17
#define BTV_TimeStamp 18
#define BTV_WindowOpen 19
#define BTV_Action 20
#define BTV_Message 21
#define BTV_PluginMsg 22
#define BTV_ServerWelcome 23
#define BTV_Background 24
#define BTV_ModeChange 25
#define BTV_Highlight 26
#define BTV_HighlightText 27
#define BTV_Whois 28
#define BTV_Invite 29
#define BTV_Wallops 30
#define BTV_NUM_COLS 31
// also put these names in options.c and btextview.c

#ifndef BERSIRC_PLUGIN
typedef struct btvla
{
	int x1, y1;
	int x2, y2;
	char *url;
	
	struct btvla *next;
} btv_linkarea;

typedef struct btvl {
	char *text;
	int colour;
	int time;
	int flags;

	int height;

	int validated;
	int shown;
} BTVLine;

typedef struct BTVS
{
	CWidget *w;
	
	BTVLine *lines_data;
	int lines_size;
	int lines_num;
	
	btv_linkarea *links;
	
	int hl_mode;
	int hl_mx, hl_my;
	int hl_msx, hl_msy;
	int hl_s_line;
	int hl_s_pos;
	int hl_e_line;
	int hl_e_pos;
	
	struct BTVS *next;
	struct BTVS *prev;
} BTextView;

extern BTextView *btv_head;
extern BTextView *btv_curr;

BTextView *btv_by_widget( CWidget *w );
CWidget *c_btv( CWidget *parent, int x, int y, int width, int height, int flags );
void c_btv_addline( BTextView *btv, char *line, int flags, int col );
void c_btv_clear( BTextView *btv );
BTVLine *c_btv_getline( BTextView *btv, int l );
int c_btv_printf( BTextView *btv, int flags, int col, char *fmt, ... );
#endif

//
