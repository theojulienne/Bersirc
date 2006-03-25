/*************************************************************************
$Id: irc.c 188 2005-11-05 00:43:35Z terminal $

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

#define QMT BERSTAG

char *quitMessages[] = {
	"\002Bersirc 2.2\002: Looks, feels and sounds (?!) different!" QMT,
	"Wow! What a great client! \002Bersirc 2.2\002" QMT,
	"What?! Open source isn't good enough for you? \002Bersirc 2.2\002" QMT,
	"\002Bersirc 2.2\002: All the original sexiness of Bersirc, open to the world." QMT,
	"Get out of that boring IRC client! It's no good for you. \002Bersirc 2.2\002 is your answer!" QMT,
	"\"Help! I've been g:lined from my mIRC!!\" \002Bersirc 2.2\002: less n00bs" QMT,
// back once it's x-platform ;)	"Think cross platform IRC client's never look nice? Think again! \002Bersirc 2.2\002" QMT,
	"I don't like you. But \002Bersirc 2.2\002 does. Try it out now." QMT,
	"Quick! Kill your client! \002Bersirc 2.2\002 is here!" QMT,
	"Think your current client is sexy? Check out \002Bersirc 2.2\002!" QMT,
	"The Bersirc are coming! The Bersirc are coming!" QMT,
	"Yummy, like ircing on a cake!" QMT,
	"\002Bersirc 2.2\002, for external use only." QMT,
	0,
};

#define TXTSZ 17

char *b_get_quit_message( )
{
	char *qmsg = "";
	int a, b;
	
	qmsg = b_get_option_string( xidentity, "general", "opt_misc_quit_message" );
	
	if ( qmsg == NULL || !strcmp( qmsg, "" ) )
	{
		for ( a = 0; quitMessages[a] != 0; a++ )
			;
		
		srand( time( NULL ) );
		b = rand( ) % a;
		
		return quitMessages[b];
	}
	
	return qmsg;
}

void b_identity_attrib( char *attrib, char *buf )
{
	char *txt;
	
	txt = c_xml_attrib_get( xidentity, attrib );
	
	if ( txt == 0 )
		return;
	
	strcpy( buf, txt );
}

int b_fontd_by_loc_from_xml( BFontDef *font, char *loc )
{
	XMLItem *fonts;
	char *xlocn;
	char *size, *face;
	
	strcpy( font->face, "Verdana" );
	font->size = 16;
	
	fonts = c_xml_find_child( xidentity, "fonts" );
	
	if ( fonts == 0 )
		return 0;
	
	for ( fonts = fonts->child_head; fonts; fonts = fonts->next )
	{
		if ( strcasecmp( fonts->name, "font" ) )
			continue;
		
		xlocn = 0;
		xlocn = c_xml_attrib_get( fonts, "location" );
		
		if ( xlocn == 0 )
			continue;
		
		if ( !strcasecmp( xlocn, loc ) )
		{
			// got it.. fill font def
			size = c_xml_attrib_get( fonts, "size" );
			if ( size == 0 )
				size = "16";
			
			face = c_xml_attrib_get( fonts, "face" );
			if ( face == 0 )
				face = "Verdana";
			
			strcpy( font->face, face );
			font->size = atoi( size );
			
			// need more here
			
			return 1;
		}
	}
	
	return 0;
}

void b_widget_set_font( object_t *w, char *loc )
{
	BFontDef fontd;
	
	if ( w == 0 )
		return;
	
	b_fontd_by_loc_from_xml( &fontd, loc );
	
	/* FIXME: someone asked that Bold (weight) be allowed as an option */
	widget_set_font( WIDGET(w), fontd.face, fontd.size, cFontWeightNormal, cFontSlantNormal, cFontDecorationNormal );
}

//
