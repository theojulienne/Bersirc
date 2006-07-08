/*************************************************************************
$Id: xml.c 150 2005-08-10 01:03:47Z terminal $

Claro - A cross platform GUI toolkit which "makes sense".
Copyright (C) 2004 Theo Julienne and Gian Perrone

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**************************************************************************/

#include "../includes.h"

/***

 This really should be made part of Claro in the way originally intended,
 which was to have "claro.dll", "clarogt.dll", "clarost.dll", etc.

 ***/

XMLFile *c_xml_create( )
{
	XMLFile *xf;
	
	if ( !( xf = (XMLFile *)malloc( sizeof( XMLFile ) ) ) )
		return NULL;
	
	memset( xf, 0, sizeof( XMLFile ) );
	
	return xf;
}

void c_xml_destroy( XMLFile *xf )
{
	c_xml_clean( xf );
	free( xf );
}

void c_xml_clean( XMLFile *xf )
{
	c_xml_clean_from( xf->root_node.child_head );
	xf->root_node.child_head = xf->root_node.child_curr = 0;
}

void c_xml_clean_from( XMLItem *start )
{
	XMLItem *curr, *next;
	
	for ( curr = start; curr != 0; curr = next )
	{
		next = curr->next;
		
		c_xml_clean_from( curr->child_head );
		
		c_xml_clean_attribs( curr );
		
		free( curr->name );
		free( curr );
	}
}

void c_xml_clean_attribs( XMLItem *item )
{
	XMLAttrib *curr, *next;
	
	for ( curr = item->attrib_head; curr != 0; curr = next )
	{
		next = curr->next;
		
		free( curr->name );
		free( curr->value );
		free( curr );
	}
}

XMLItem *c_xml_add_child( XMLItem *item, char *name )
{
	XMLItem *newitem;
	int a;
	
	if ( !( newitem = (XMLItem *)malloc( sizeof( XMLItem ) ) ) )
		return NULL;
	
	memset( newitem, 0, sizeof( XMLItem ) );
	
	newitem->track = 1; // it's changed, obviously!
	
	if ( item->child_curr != 0 )
	{
		item->child_curr->next = newitem;
		newitem->prev = item->child_curr;
		item->child_curr = newitem;
	}
	else
	{
		item->child_head = item->child_curr = newitem;
	}
	
	a = strlen( name ) + 1;
	
	if ( !( newitem->name = (char *)malloc( a ) ) )
		return newitem;
	
	strncpy( newitem->name, name, a );
	
	return newitem;
}

XMLItem *c_xml_find_child( XMLItem *parent, char *name )
{
	XMLItem *curr;
	
	for ( curr = parent->child_head; curr != 0; curr = curr->next )
	{
		if ( curr->track == 2 || curr->name == 0 )
			continue;
		
		if ( !strcasecmp( curr->name, name ) )
			return curr;
	}
	
	return NULL;
}

// converts dot-delimited "path" to an XMLItem, and creates any missing
// items along the way (no matter how far deep/uncreated it is currently)
XMLItem *c_xml_get_path_safe( XMLFile *file, char *inpath )
{
	char *tmp;
	char *path;
	XMLItem *curr, *next;
	int a, b, c;
	
	a = strlen( inpath );
	
	tmp = (char *)malloc( a + 1 );
	path = (char *)malloc( a + 2 );
	
	strcpy( path, inpath );
	
	// prepend "." to force update
	strcat( path, "." );
	
	curr = c_xml_root( file );
	
	for ( b = 0, c = 0; b <= a; b++, c++ )
	{
		tmp[c] = 0;
		
		if ( path[b] != '.' )
			tmp[c] = path[b];
		else
		{
			if ( ( next = c_xml_find_child( curr, tmp ) ) == NULL )
				curr = c_xml_add_child( curr, tmp );
			else
				curr = next;
			
			c = -1;
		}
	}
	
	free( tmp );
	
	return curr;
}

XMLItem *c_xml_get_path( XMLFile *file, char *inpath )
{
	char *tmp;
	char *path;
	XMLItem *curr, *next;
	int a, b, c;
	
	a = strlen( inpath );
	
	tmp = (char *)malloc( a + 1 );
	path = (char *)malloc( a + 2 );
	
	strcpy( path, inpath );
	
	// prepend "." to force update
	strcat( path, "." );
	
	curr = c_xml_root( file );
	
	for ( b = 0, c = 0; b <= a; b++, c++ )
	{
		tmp[c] = 0;
		
		if ( path[b] != '.' )
			tmp[c] = path[b];
		else
		{
			if ( ( next = c_xml_find_child( curr, tmp ) ) == NULL )
			{
				free( tmp );
				free( path );
				return 0;
			}
			else
				curr = next;
			
			c = -1;
		}
	}
	
	free( tmp );
	free( path );
	
	return curr;
}

XMLAttrib *c_xml_find_attrib( XMLItem *a, char *name )
{
	XMLAttrib *curr;
	
	for ( curr = a->attrib_head; curr != 0; curr = curr->next )
	{
		if ( curr->name == 0 )
			continue;
		
		if ( !strcasecmp( curr->name, name ) )
			return curr;
	}
	
	return NULL;
}

XMLAttrib *c_xml_create_attrib( XMLItem *item, char *name )
{
	XMLAttrib *newattr;
	//int a;
	
	if ( !( newattr = (XMLAttrib *)malloc( sizeof( XMLAttrib ) ) ) )
		return NULL;
	
	memset( newattr, 0, sizeof( XMLAttrib ) );
	
	if ( item->attrib_curr != 0 )
	{
		item->attrib_curr->next = newattr;
		newattr->prev = item->attrib_curr;
		item->attrib_curr = newattr;
	}
	else
	{
		item->attrib_head = item->attrib_curr = newattr;
	}
	/*
	a = strlen( name ) + 1;
	
	if ( !( newattr->name = (char *)malloc( a ) ) )
		return newattr;
	
	strncpy( newattr->name, name, a );
	*/
	
	newattr->name = strdup( name );
	
	return newattr;
}

void c_xml_attrib_set( XMLItem *a, char *name, char *value )
{
	XMLAttrib *curr;
	
	a->track = 1;
	
	if ( ( curr = c_xml_find_attrib( a, name ) ) == NULL )
	{
		curr = c_xml_create_attrib( a, name );
	}
	
	if ( curr->value != 0 )
		free( curr->value );
	/*
	curr->value = (char *)malloc( strlen( value ) + 1 );
	
	strcpy( curr->value, value );*/
	curr->value = strdup( value );
}

char *c_xml_attrib_get( XMLItem *a, char *name )
{
	XMLAttrib *curr;
	
	if ( ( curr = c_xml_find_attrib( a, name ) ) == NULL )
		return 0;
	
	return curr->value;
}

void c_xml_parse_attribs( XMLItem *item, char *str, int overwrite )
{
	char name[1024];
	char value[1024];
	char chr[10];
	int a, b, c, d, e, f;
	
	b = strlen( str );
	
	if ( str[strlen(str)-1] == '/' )
		b--;
	
	for ( a = 0, c = 0, e = 0; a < b; a++ )
	{
		if ( c == 0 )
		{
			// waiting for an attribute...
			
			if ( str[a] == ' ' || str[a] == '\t' || str[a] == '\r' || str[a] == '\n' )
				continue;
			else
			{
				c = 1;
				d = 0;
			}
		}
		
		if ( c == 1 )
		{
			if ( str[a] == '=' )
			{
				if ( str[a+1] == '"' )
					a++;
				c = 2;
				d = 0;
				continue;
			}
			
			name[d] = str[a];
			d++;
			name[d] = 0;
		}
		
		if ( c == 2 )
		{
			if ( str[a] == '"' )
			{
				c = 0;
				
				// save
				c_xml_attrib_set( item, name, value );
				value[0] = 0;
				
				continue;
			}
			
			if ( e == 0 )
			{
				if ( str[a] == '&' )
				{
					e = 1;
					f = 0;
				}
				else
				{
					value[d] = str[a];
					d++;
					value[d] = 0;
				}
			}
			else
			{
				if ( str[a] == ';' )
				{
					e = 0;
					if ( !strcasecmp( chr, "amp" ) )
					{
						value[d] = '&';
						d++;
						value[d] = 0;
					}
				}
				else
				{
					chr[f] = str[a];
					f++;
					chr[f] = 0;
				}
			}
		}
	}
}

int c_xml_merge_file_run( FILE *fp, int overwrite, XMLItem *parent )
{
	char bufa[4096];
	char bufb[4096];
	char buf[8192];
	char *nameptr;
	int b, d, e, f, g, h;
	char c;
	XMLItem *next;
	
	b = 0;
	h = 0;
	
	while ( !feof( fp ) )
	{
		c = fgetc( fp );
		
		if ( b == 0 )
		{
			// waiting for a tag
			if ( c != '<' )
				continue;
			else
			{
				b = 1;
				d = 0;
				e = 0;
				f = 0;
				bufa[d] = 0;
			}
		}
		else if ( b == 1 )
		{
			// inside a tag
			if ( e == 0 && c == ' ' )
			{
				e = 1;
				f = 0;
				bufa[d] = 0;
				bufb[f] = 0;
			}
			else if ( c != '>' )
			{
				if ( e == 0 )
				{
					bufa[d] = c;
					d++;
				}
				else
				{
					bufb[f] = c;
					f++;
				}
			}
			else
			{
				bufa[d] = 0;
				bufb[f] = 0;
				
				g = 0;
				
				if ( bufa[0] == '!' )
					g = 1;
				else if ( bufa[0] == '?' )
					g = 2;
				
				nameptr = bufa;
				
				if ( g > 0 )
				{
					nameptr++; // remove leading ? or !
					sprintf( buf, "%s %s", nameptr, bufb );
					nameptr = buf;
				}
				
				if ( g == 2 )
					bufb[f-1] = 0; // remove trailing ?
				
				if ( bufa[0] != '/' )
				{
					next = c_xml_add_child( parent, nameptr );
					
					next->type = g;
				
					if ( g == 0 )
						c_xml_parse_attribs( next, bufb, overwrite );
					else
					{
						//printf( "Got comment <%c%s>\n", (g==1?'!':'?'), buf );
						b = 0;
						continue;
					}
				}
				
				if ( bufb[f-1] == '/' )
				{
					// single line tag
					b = 0;
					//printf( "XML Tag Parsed inside '%s' : %s [%s]\n", bufa, bufb, parent->name );
					h++;
				}
				else
				{
					// contains other tags
					b = 0;
					h++;
					
					if ( bufa[0] == '/' )
						break;
					
					//printf( "Parent tag '%s' encountered inside '%s'!\n", bufa, parent->name );
					if ( c_xml_merge_file_run( fp, overwrite, next ) > 0 )
						next->track = 1;
				}
			}
		}
	}
	
	return h;
}

// overwrite == 1 : overwrites current values
int c_xml_merge_file( XMLFile *file, char *path, int overwrite )
{
	FILE *fp;
	
	if ( !( fp = fopen( path, "r" ) ) )
		return 0;
	
	c_xml_merge_file_run( fp, overwrite, c_xml_root( file ) );
	
	fclose( fp );
	
	return 1;
}

int c_xml_load_file( XMLFile *file, char *path )
{
	c_xml_clean( file );
	return c_xml_merge_file( file, path, 1 );
}

void c_xml_dump_file_run( FILE *file, int indent, XMLItem *root )
{
	XMLItem *curr;
	XMLAttrib *ca;
	int a, b;
	
	for ( curr = root->child_head; curr != 0; curr = curr->next )
	{
		for ( a = 0; a < indent; a++ )
			fprintf( file, "\t" );
		
		if ( curr->type == 0 )
		{
			fprintf( file, "<%s", curr->name );
			
			for ( ca = curr->attrib_head; ca != 0; ca = ca->next )
			{
				fprintf( file, " %s=\"", ca->name );
				for ( b = 0; b < strlen( ca->value ); b++ )
				{
					if ( ca->value[b] == '&' )
						fprintf( file, "&amp;" );
					else
						fprintf( file, "%c", ca->value[b] );
				}
				fprintf( file, "\"" );
			}
			
			if ( curr->child_head == 0 )
				fprintf( file, " />\n" );
			else
			{
				fprintf( file, ">\n" );
				c_xml_dump_file_run( file, indent + 1, curr );
				
				for ( a = 0; a < indent; a++ )
					fprintf( file, "\t" );
				fprintf( file, "</%s>\n", curr->name );
			}
		}
		else if ( curr->type == 1 )
			fprintf( file, "<!%s>\n", curr->name );
		else if ( curr->type == 2 )
			fprintf( file, "<?%s>\n", curr->name );
	}
}

int c_xml_dump_file( XMLFile *file, char *path )
{
	FILE *fp;
	
	if ( ( fp = fopen( path, "w" ) ) == NULL )
	{
		printf( "fopen('%s','w') failed, error #%d\n", path, errno );
		return -1;
	}
	
	c_xml_dump_file_run( fp, 0, c_xml_root( file ) );
	
	fclose( fp );
	
	return 1;
}

//
