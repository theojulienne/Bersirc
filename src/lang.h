/*************************************************************************
$Id: lang.h 69 2005-05-08 06:06:55Z terminal $

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

void lang_init( char *lang );
char *lang_phrase_parse( char *buf, int size, char *phrase, ... );
char *lang_phrase_quick( char *phrase );
int lang_str_overlap( char *a, char *b );

extern char lang_tmp_buf[1024];

// if the language we are using doesn't contain a phrase, fall back to this
#define LANG_FALLBACK "en_uk"

//
