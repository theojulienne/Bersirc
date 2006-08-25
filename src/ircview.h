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

#define c_btv_addline(o,t,f,c) ircview_add_line((ircview_t*)o,c,f,t)
#define c_btv_printf(i,f,c,fmt,a...) ircview_printf((ircview_t*)i,c,f,fmt, ## a)
#define BTextView ircview_t

enum
{
	bIRCViewLinkTypeURL=0,
	bIRCViewLinkTypeChannel,
	bIRCViewLinkTypePerson,
};

typedef struct
{
	/* type of link, see above */
	int type;
	
	/* link destination (URL, etc)*/
	char *link;
	
	/* offset and length of link chars inside line */
	int offset, length;
	
	/* location and size of bounding box */
	int x, y, w, h;
} ircview_link_t;

typedef struct
{
	/* contains the original text for this line */
	char *original;
	
	/* contains the text, with the timestamp */
	char *text;
	
	/* contains the text, stripped of colours and formatting */
	char *cleantext;
	
	/* contains the internal BTV_* colour code for the line */
	int colour;
	
	/* time line was received */
	int time;
	
	/* links */
	list_t links;
	int linkn;
	
	/* viewable? */
	int visible;
	
	int flags;
} ircview_line_t;

typedef struct
{
	canvas_widget_t canvas;
	
	ircview_line_t *hl_s_line;
	int hl_s_pos;
	ircview_line_t *hl_e_line;
	int hl_e_pos;
	
	object_t *scroll;
	
	list_t lines;
} ircview_t;

ircview_t *ircview_widget_create( object_t *parent, bounds_t *b );
void ircview_add_line( ircview_t *ircview, int colour, int flags, char *text );
int ircview_printf( ircview_t *ircview, int colour, int flags, char *fmt, ... );
void ircview_set_scrollbar( ircview_t *ircview, object_t *s );
void ircview_update_scroll( ircview_t *ircview );
void ircview_clear( ircview_t *ircview );

void ircview_link_test( ircview_t *ircview, ircview_line_t *line, int x, int y, int chars, int start_offset );

extern list_t ircviews;

enum
{
	BTV_ClientWelcome=0,
	BTV_ConnectionMsg,
	BTV_Error,
	BTV_PingPong,
	BTV_UserJoin,
	BTV_Info,
	BTV_UserQuit,
	BTV_UserNick,
	BTV_TopicChange,
	BTV_MOTD,
	BTV_UserPart,
	BTV_CTCP,
	BTV_Notice,
	BTV_Alert,
	BTV_Topic,
	BTV_UserKick,
	BTV_TopicTime,
	BTV_Debug,
	BTV_TimeStamp,
	BTV_WindowOpen,
	BTV_Action,
	BTV_Message,
	BTV_PluginMsg,
	BTV_ServerWelcome,
	BTV_Background,
	BTV_ModeChange,
	BTV_Highlight,
	BTV_HighlightText,
	BTV_Whois,
	BTV_Invite,
	BTV_Wallops,
	BTV_NUM_COLS,
};



