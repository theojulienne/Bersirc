/*************************************************************************
$Id: configdir.c 153 2005-08-10 01:22:11Z terminal $

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

char userdir[512];
char ntmpfile[512];

char *config_file_loc( char *name )
{
	FILE *file;
	
	sprintf( ntmpfile, "%s/%s", userdir, name );
	
	if ( ( file = fopen( ntmpfile, "r" ) ) ) {
		fclose( file );
		return ntmpfile;
	}
	
	sprintf( ntmpfile, "%s/%s", XML_FALLBACK_DIR, name );
	
	return ntmpfile;
}

#ifndef CSIDL_APPDATA
// oops for MSVC
#define CSIDL_APPDATA   26
#define SHGFP_TYPE_CURRENT 0
#endif

void config_userdir_init( )
{
#ifdef ENV_WIN32
	char path[MAX_PATH];
	
	SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, (LPTSTR)&path );
	snprintf( userdir, 512, "%s/Bersirc", path );
	
	if ( CreateDirectory( userdir, NULL ) == 0 )
	{
		if ( GetLastError( ) == ERROR_PATH_NOT_FOUND )
		{
			MessageBox( 0, "The application data directory for this user does not exist! Bersirc cannot load without it.\n\nPlease check the Bersirc website for possible solutions or work-arounds.", "Bersirc has encountered a 'Whoops'!", 0 );
			return;
		}
	}
#else
	snprintf( userdir, 512, "%s/.bersirc", getenv( "HOME" ) );

	if ( mkdir( userdir ) != 0 && errno != EEXIST )
	{
		printf( "Could not create %s!\n", userdir );
		exit( 0 );
	}
#endif
}

//
