/*************************************************************************
$Id: options.h 53 2005-04-30 05:44:32Z terminal $

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

void b_open_options( );

#define B_OPTS_PAGE_CREATE_PARMS char *title, object_t *parent, int px, int py, int pw, int ph, XMLItem *identity
#define B_OPTIONS_PAGE_CREATE(n) object_t *n( B_OPTS_PAGE_CREATE_PARMS )

#define B_OPTS_PAGE_SAVE_PARMS XMLItem *identity
#define B_OPTIONS_PAGE_SAVE(n) void n( B_OPTS_PAGE_SAVE_PARMS )

char *b_get_option_string( XMLItem *identity, char *root, char *sub );
int b_get_option_bool( XMLItem *identity, char *root, char *sub );
int b_get_option_int( XMLItem *identity, char *root, char *sub );
void b_set_option_string( XMLItem *identity, char *root, char *sub, char *value );
void b_set_option_bool( XMLItem *identity, char *root, char *sub, int value );
void b_set_option_int( XMLItem *identity, char *root, char *sub, int value );

//
