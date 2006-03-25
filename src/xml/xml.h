/*************************************************************************
$Id: xml.h 52 2005-04-30 04:32:24Z terminal $

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

#define c_xml_root( a ) &a->root_node

typedef struct xmlattrib_struct
{
	char *name;
	char *value;
	
	struct xmlattrib_struct *next;
	struct xmlattrib_struct *prev;
} XMLAttrib;

typedef struct xmlitem_struct
{
	char *name;
	
	int type;  // 0: Normal Item
	           // 1: Comment (<!--name-->)
	           // 2: Definition (<?name?>)
	
	int track; // 0: Unchanged since load
	           // 1: Modified since load
	           // 2: Removed. Gets ignored.
	
	XMLAttrib *attrib_head;
	XMLAttrib *attrib_curr;
	
	struct xmlitem_struct *child_head;
	struct xmlitem_struct *child_curr;
	
	struct xmlitem_struct *next;
	struct xmlitem_struct *prev;
} XMLItem;

typedef struct
{
	XMLItem root_node;
} XMLFile;

//

void c_xml_clean_attribs( XMLItem *item );
void c_xml_clean_from( XMLItem *start );
void c_xml_clean( XMLFile *xf );
void c_xml_destroy( XMLFile *xf );
XMLFile *c_xml_create( );
XMLItem *c_xml_add_child( XMLItem *item, char *name );
XMLItem *c_xml_find_child( XMLItem *parent, char *name );
XMLItem *c_xml_get_path_safe( XMLFile *file, char *path );
XMLItem *c_xml_get_path( XMLFile *file, char *inpath );
XMLAttrib *c_xml_find_attrib( XMLItem *a, char *name );
XMLAttrib *c_xml_create_attrib( XMLItem *item, char *name );
char *c_xml_attrib_get( XMLItem *a, char *name );
void c_xml_attrib_set( XMLItem *a, char *name, char *value );
int c_xml_merge_file( XMLFile *file, char *path, int overwrite );
int c_xml_load_file( XMLFile *file, char *path );
int c_xml_dump_file( XMLFile *file, char *path );

//
