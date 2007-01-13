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

#include "includes.h"

block_heap_t *line_heap = 0;

list_t ircviews;

char *irccolours[] = {
	"#ffffff",
	"#000000",
	"#000080",
	"#008000",
	"#ff0000",
	"#800000",
	"#800080",
	"#ff6600",
	"#ffff00",
	"#00ff00",
	"#0099cc",
	"#00ffff",
	"#0000ff",
	"#ff00ff",
	"#808080",
	"#c0c0c0",
};

char *ircview_colour_names[BTV_NUM_COLS] = {
	"ClientWelcome",
	"ConnectionMsg",
	"Error",
	"PingPong",
	"UserJoin",
	"Info",
	"UserQuit",
	"UserNick",
	"TopicChange",
	"MOTD",
	"UserPart",
	"CTCP",
	"Notice",
	"Alert",
	"Topic",
	"UserKick",
	"TopicTime",
	"Debug",
	"TimeStamp",
	"WindowOpen",
	"Action",
	"Message",
	"PluginMsg",
	"ServerWelcome",
	"Background",
	"ModeChange",
	"Highlight",
	"HighlightText",
	"Whois",
	"Invite",
	"Wallops",
};

char *ircview_cache_cols[BTV_NUM_COLS];

int ircview_cache_colours_extracted = 0;

void ircview_cache_colours_update( )
{
	int a;
	
	for ( a = 0; a < BTV_NUM_COLS; a++ )
	{
		ircview_cache_cols[a] = b_get_option_string( xidentity, "colours", ircview_colour_names[a] );
	}

	ircview_cache_colours_extracted = 1;
}

char irc_fmt_chars[] = "\002\037\035\003\015\004\017\026";

int ircview_do_fmt( const char *fmt_ptr, ircview_line_t *line );

void irc_fmt_strip( char *dst, const char *src )
{
	while ( *src )
	{
		while ( *src && strchr( irc_fmt_chars, *src ) != NULL )
		{
			int b;
			
			b = ircview_do_fmt( src, 0 );
			
			src += b + 1;
		}
		
		*(dst++) = *(src++);
	}
	
	*dst = 0;
}

int calc_best_end( const char *text, int max )
{
	//int a = max;
	const char *lptext;
	
	lptext = text + max;
	
	while ( lptext != text && *lptext != ' ' )
		lptext--;
	
	/* if we didn't find a space, return the original. */
	if ( lptext == text )
		return max;
	
	/* otherwise, return the length to the space */
	return lptext - text + 1;
}

int mouse_sel = 0;
int mouse_startx = 0, mouse_starty = 0;
int mouse_endx = 0, mouse_endy = 0;

event_handler( ircview_handle_rightrel )
{
	int x = event_get_arg_int( event, 0 );
	int y = event_get_arg_int( event, 1 );
	BServerWindow *sw;
	BChatWindow *cw;
	object_t *popup;
	int dx, dy;
	
	sw = 0;
	cw = 0;
	
	if ( ( sw = b_find_server_by_widget( object ) ) == NULL )
	{
		if ( ( cw = b_find_chat_by_widget( object ) ) == NULL )
			return;
	}
	
	popup = 0;
	
	if ( sw != 0 )
		popup = sw->conmenu.menu;
	else if ( cw != 0 )
		popup = cw->conmenu.menu;
	
	if ( popup != 0 )
	{
		widget_screen_offset( object, &dx, &dy );
		x += dx;
		y += dy;
		menu_popup( popup, x, y, cMenuPopupAtCursor );
	}
}

ircview_link_t *ircview_find_link_at( ircview_t *iv, int cx, int cy )
{
	node_t *n, *ln;
	
	LIST_FOREACH( n, iv->lines.head )
	{
		ircview_line_t *line = (ircview_line_t *)n->data;
		
		if ( line->visible == 0 || line->linkn == 0 )
			continue;
		
		LIST_FOREACH( ln, line->links.head )
		{
			ircview_link_t *link = (ircview_link_t *)ln->data;
			
			if ( cx >= link->x && cx <= link->x + link->w &&
			     cy >= link->y && cy <= link->y + link->h )
			{
				return link;
			}
		}
	}
	
	return NULL;
}

void ircview_handle_mousedown( object_t *obj, event_t *event )
{
	int cx = event_get_arg_int( event, 0 );
	int cy = event_get_arg_int( event, 1 );
	ircview_link_t *link = NULL;
	
	if ( ( link = ircview_find_link_at( (ircview_t*)obj, cx, cy ) ) != NULL)
	{
		char cmd[4096];
		if ( link->type == bIRCViewLinkTypeURL )
			sprintf( cmd, "/go %s", link->link );
		else if ( link->type == bIRCViewLinkTypeChannel )
			sprintf( cmd, "/join %s", link->link );
		else if ( link->type == bIRCViewLinkTypePerson )
			sprintf( cmd, "/query %s", link->link );
		else
			return;
		
		b_window_command( b_find_any_by_widget(obj), cmd );
		return;
	}
	
	mouse_sel = 1;
	mouse_startx = cx;
	mouse_starty = cy;
	mouse_endx = cx;
	mouse_endy = cy;
	
	canvas_redraw( OBJECT(obj) );
}

void ircview_handle_mouseup( object_t *obj, event_t *event )
{
	ircview_t *iv = (ircview_t *)obj;
	ircview_line_t *ivl;
	node_t *n;
	int in_hl=0;
	char *text = 0, *tmp, *rtmp;
	int tsz = 0;
	
	if ( mouse_sel == 1 && iv->hl_s_line != 0 && iv->hl_e_line != 0 )
	{
		LIST_FOREACH( n, iv->lines.head )
		{
			ivl = (ircview_line_t *)n->data;
			
			if ( ivl == iv->hl_s_line )
				in_hl = 1;
			
			if ( in_hl == 1 )
			{
				rtmp = tmp = strdup( ivl->text );
				
				if ( ivl == iv->hl_e_line )
				{
					tmp[iv->hl_e_pos] = 0;
				}
				
				tsz += strlen( tmp ) + 3;
				text = (char *)realloc( text, tsz );
				
				if ( ivl == iv->hl_s_line )
				{
					strcpy( text, "" );
					tmp += iv->hl_s_pos;
				}
				
				strcat( text, tmp );
				
				free( rtmp );
				
				if ( ivl != iv->hl_e_line )
#ifdef _WIN32
					strcat( text, "\r\n" );
#else
					strcat( text, "\n" );
#endif
			}
			
			if ( ivl == iv->hl_e_line )
				break;
		}
		
		clipboard_set_text( WIDGET(obj), text );
		
		free( text );
	}
	
	mouse_sel = 0;
	
	canvas_redraw( OBJECT(obj) );
}

void ircview_handle_mousemove( object_t *obj, event_t *event )
{
	int cx = event_get_arg_int( event, 0 );
	int cy = event_get_arg_int( event, 1 );
	ircview_t *iv = (ircview_t *)obj;
	
	mouse_endx = cx;
	mouse_endy = cy;
	
	if ( mouse_sel == 1 )
	{
		canvas_redraw( OBJECT(obj) );
		return;
	}
	
	if ( ircview_find_link_at( iv, cx, cy ) != NULL )
		widget_set_cursor( obj, cCursorPoint );
	else
		widget_set_cursor( obj, cCursorNormal );
}

int iv_render_hl = 0;
char iv_render_fg[8];
char iv_render_bg[8];

int iv_render_bold = 0;
int iv_render_italic = 0;
int iv_render_underline = 0;

int iv_line_fmt_offset = 0;
int iv_line_current_w = 0;

#define IRCCH_BOLD '\002'
#define IRCCH_UNDERLINE '\037'
#define IRCCH_ITALIC '\035'
#define IRCCH_REVERSE '\026'
#define IRCCH_RESET '\015'
#define IRCCH_HEXCOL '\004'
#define IRCCH_COLOR '\003'

void ircview_set_colour_hex( ircview_t *ircview, const char *hex, int bg )
{
	float r, g, b;
	int ir, ig, ib;
	
	ir = ig = ib;
	r = g = b = 0.0f;
	
	sscanf( hex, "#%2x%2x%2x", &ir, &ig, &ib );
	
	r = ir / 255.0f;
	g = ig / 255.0f;
	b = ib / 255.0f;
	
	if ( bg == 1 )
		canvas_set_text_bgcolor( OBJECT(ircview), r, g, b, 1 );
	else
		canvas_set_text_color( OBJECT(ircview), r, g, b, 1 );
}

int ircview_do_fmt( const char *fmt_ptr, ircview_line_t *line )
{
	int extras = 0;
	char coltmp[8];
	
	/* formatting char! */
	switch ( *fmt_ptr )
	{
		case IRCCH_BOLD:
			iv_render_bold = !iv_render_bold;
			break;
		case IRCCH_UNDERLINE:
			iv_render_underline = !iv_render_underline;
			break;
		case IRCCH_ITALIC:
			iv_render_italic = !iv_render_italic;
			break;
		case IRCCH_REVERSE:
			strcpy( coltmp, iv_render_fg );
			strcpy( iv_render_fg, iv_render_bg );
			strcpy( iv_render_bg, coltmp );
			break;
		case IRCCH_RESET:
			if ( line != 0 )
				strcpy( iv_render_fg, ircview_cache_cols[line->colour] );
			strcpy( iv_render_bg, ircview_cache_cols[BTV_Background] );
			
			iv_render_bold = 0;
			iv_render_italic = 0;
			iv_render_underline = 0;
			
			break;
		case IRCCH_HEXCOL:
			break;
		case IRCCH_COLOR:
		{
			int col = -2;
			int i = 0;
			
			if ( fmt_ptr[1] == '0' ) {
				// this could be a 0-padded number. is it?
				if ( fmt_ptr[2] >= '0' && fmt_ptr[2] <= '9' ) {
					// we'll treat this as a 2-digit 0-padded number.
					col = fmt_ptr[2] - '0';
					i += 2;
				} else {
					// they meant colour 0.
					col = 0;
					i += 1;
				}
			} else if ( fmt_ptr[1] == '1' ) {
				// this could be a number > 9. is it?
				if ( fmt_ptr[2] >= '0' && fmt_ptr[2] <= '5' ) {
					// we'll treat this as a 2-digit number between 9 and 15.
					col = 10 + fmt_ptr[2] - '0';
					i += 2;
				} else {
					// they meant colour 1.
					col = 1;
					i += 1;
				}
			} else if ( fmt_ptr[1] == '9' && fmt_ptr[2] == '9' ) {
				col = -1;
				i += 2;
			} else if ( fmt_ptr[1] >= '2' && fmt_ptr[1] <= '9' ) {
				// this is a normal number, that CANNOT have a second digit.
				col = fmt_ptr[1] - '0';
				i += 1;
			} else {
				// this is a colour reset, as some clients
				// treat an empty colour number as such.
				col = -1;
			}
			
			extras += i;
			
			i =0;
			
			if ( col == -1 ) {
				if ( line != 0 )
					strcpy( iv_render_fg, ircview_cache_cols[line->colour] );
				strcpy( iv_render_bg, ircview_cache_cols[BTV_Background] );
			
				break; // we don't want to check for setting of background :)
			} else {
				strcpy( iv_render_fg, ircview_cache_cols[col] );
			}
	
			if ( fmt_ptr[1] == ',' ) {
				// background too!
				col = -2;
				
				if ( fmt_ptr[2] == '0' ) {
					// this could be a 0-padded number. is it?
					if ( fmt_ptr[3] >= '0' && fmt_ptr[3] <= '9' ) {
						// we'll treat this as a 2-digit 0-padded number.
						col = fmt_ptr[3] - '0';
						i += 2;
					} else {
						// they meant colour 0.
	 					col = 0;
						i += 1;
					}
				} else if ( fmt_ptr[2] == '1' ) {
					// this could be a number > 9. is it?
					if ( fmt_ptr[3] >= '0' && fmt_ptr[3] <= '5' ) {
						// we'll treat this as a 2-digit number between 9 and 15.
						col = 10 + fmt_ptr[3] - '0';
						i += 2;
					} else {
						// they meant colour 1.
						col = 1;
						i += 1;
					}
				} else if ( fmt_ptr[2] >= '2' && fmt_ptr[2] <= '9' ) {
					// this is a normal number, that CANNOT have a second digit.
					col = fmt_ptr[2] - '0';
					i += 1;
				} else {
					// eek? :)
					return extras;
				}
				
				if ( col != -1 ) {
					strcpy( iv_render_bg, ircview_cache_cols[col] );
				}
			}
			
			extras += i;
		
			break;
		}
	}
	
	return extras;
}

int ircview_paintmode = 0;

int ircview_update_font( ircview_t *ircview )
{
	if ( iv_render_hl == 0 )
	{
		ircview_set_colour_hex( ircview, iv_render_fg, 0 );
		ircview_set_colour_hex( ircview, iv_render_bg, 1 );
	}
	else
	{
		ircview_set_colour_hex( ircview, ircview_cache_cols[BTV_HighlightText], 0 );
		ircview_set_colour_hex( ircview, ircview_cache_cols[BTV_Highlight], 1 );
	}
	
	canvas_set_text_font( OBJECT(ircview), WIDGET(ircview)->font.face, WIDGET(ircview)->font.size, 
		(iv_render_bold		? cFontWeightBold : cFontWeightNormal ),
		(iv_render_italic	? cFontSlantItalic : cFontSlantNormal ),
		(iv_render_underline? cFontDecorationUnderline : cFontDecorationNormal ) );

	return 0;
}

#define X_PADDING 2

int ircview_show_text( ircview_t *ircview, int x, int y, int max, int *offset, ircview_line_t *line,
						int is_hl, int is_hls, int is_hle, int fx, int sx )
{
	static char tmp[16384];
	char *tmp_ptr;
	int tmp_len = 0;
	char *fmt_ptr = line->text + *offset;
	int o = 0;
	int a, b, c, d, fxo=0, xo;
	int last_print = 0, still_inc = 0;
	int hl_run_count = 0;
	int hl_os_starting = -1;
	int hl_os_ending = -1;
	int return_ready = 0;
	
	int start_pos = *offset;
	int begin_pos = start_pos;
	
	//int len = strlen( fmt_ptr );
	
	if ( is_hl || (is_hle&&!is_hls) )
		iv_render_hl = 1;
	
	for ( a = 0; *fmt_ptr; a++ )
	{
		/* handle formatting, until we hit something else */
		while ( *fmt_ptr != 0 && strchr( irc_fmt_chars, *fmt_ptr ) != NULL )
		{
			/* anything to display? */
			if ( tmp_len > 0 )
			{
				ircview_update_font( ircview );
				
				tmp[tmp_len] = 0;
				tmp_ptr = tmp;
				
				xo = canvas_text_box_width( OBJECT(ircview), tmp, tmp_len );
				
				hl_os_starting = -1;
				hl_os_ending = -1;
				return_ready = 0;
				
				/* whole line hl'd? no worries.. */
				if ( !is_hl && ircview_paintmode )
				{
					/* does highlighting start/end within this line? */
					if ( is_hls || is_hle )
					{
						int block_startx = x + fxo;
						int block_endx = block_startx + xo;
						
						if ( hl_run_count == 0 && block_startx <= fx && fx <= block_endx )
						{
							/* okay, well 'fx' contains the first coord to look at */
							
							b = canvas_text_display_count( OBJECT(ircview), tmp, fx - fxo );
							
							hl_os_starting = b;
							
							if ( !is_hls && tmp[hl_os_starting] != 0 )
								hl_os_starting++;
							
							hl_run_count++;
						}
						
						if ( hl_run_count == 1 && block_startx <= sx && sx <= block_endx )
						{
							/* okay, well 'sx' contains the second coord to look at */
							
							b = canvas_text_display_count( OBJECT(ircview), tmp, sx - fxo );
							
							hl_os_ending = b;
							
							if ( tmp[hl_os_ending] != 0 )
								hl_os_ending++;
							
							hl_run_count++;
						}
					}
				}
				
				if ( x+fxo+xo > max )
				{
					/* this would take us over the end. time to word wrap! */
					
					/* find how many characters we can fit */
					b = c = canvas_text_display_count( OBJECT(ircview), tmp, max - (fxo + X_PADDING) );
					
					/* if just one more would mean a space, to hell with it, let's add it. */
					if ( tmp[c] == ' ' )
					{
						b++;
						c++;
					}
					
					/* unless we're already ending on a space, find a space to end on */
					if ( tmp[c-1] != ' ' && tmp[c] != 0 )
					{
						/* we know we can't fit them all, so try and find a space */
						b = calc_best_end( tmp, b );
						
						if ( b == c )
						{
							/* we couldn't find something better.. would we be able to fit
							 * it all on a new line if we wrapped now? */
							
							if ( xo+X_PADDING <= max )
							{
								/* yes, we could fit it. let's do that. it looks better. */
								*offset += last_print;
								
								c = 0;
								
								return_ready = 1;
							}
						}
					}
					
					if ( return_ready == 0 )
						return_ready = 2;
				}
				
				if ( return_ready == 0 )
					b = tmp_len;
				
				
				/* highlighting? :) remember, this doesn't happen in non-draw mode */
				if ( hl_os_starting != -1 )
				{
					/* we're starting/ending highlighting (round 1) before the end */
					
					if ( is_hls )
						iv_render_hl = 0;
					else
						iv_render_hl = 1;
					ircview_update_font( ircview );
					
					if ( hl_os_starting <= b )
					{
						/* we start before the end, so split and render */
						
						canvas_show_text( OBJECT(ircview), x+fxo, y, tmp_ptr, hl_os_starting );
						d = canvas_text_box_width( OBJECT(ircview), tmp_ptr, hl_os_starting );
						fxo += d;
						xo -= d;
						
						tmp_ptr += hl_os_starting;
						b -= hl_os_starting;
						
						if ( hl_os_ending != -1 )
							hl_os_ending -= hl_os_starting;
						
						if ( return_ready != 0 )
							*offset += hl_os_starting; /* make up for shortening 'b' */
					}
					
					if ( is_hls )
					{
						/* starting highlighting now !! */
						if ( return_ready != 0 )
							ircview->hl_s_pos = *offset;
						else
							ircview->hl_s_pos = *offset + hl_os_starting;
						ircview->hl_s_pos += last_print;
						//printf( "%d\n", ircview->hl_s_pos );
						iv_render_hl = 1;
					}
					else
					{
						/* ending highlighting now !! */
						if ( return_ready != 0 )
							ircview->hl_e_pos = *offset;
						else
							ircview->hl_e_pos = *offset + hl_os_starting;
						ircview->hl_e_pos += last_print;
						//printf( " -> %d\n", ircview->hl_e_pos );
						iv_render_hl = 0;
					}
					ircview_update_font( ircview );
				}
				
				if ( hl_os_ending != -1 )
				{
					/* we're ending highlighting (round 2) before the end */
					
					if ( hl_os_ending <= b )
					{
						/* we start before the end, so split and render */
						
						canvas_show_text( OBJECT(ircview), x+fxo, y, tmp_ptr, hl_os_ending );
						d = canvas_text_box_width( OBJECT(ircview), tmp_ptr, hl_os_ending );
						fxo += d;
						xo -= d;
						
						tmp_ptr += hl_os_ending;
						b -= hl_os_ending;
						
						if ( return_ready != 0 )
							*offset += hl_os_ending; /* make up for shortening 'b' */
					}
					
					/* ending highlighting now !! */
					if ( return_ready != 0 )
						ircview->hl_e_pos = *offset;
					else
						ircview->hl_e_pos = *offset + hl_os_ending;
					if ( hl_os_starting != -1 )
						ircview->hl_e_pos += hl_os_starting;
					ircview->hl_e_pos += last_print;
					//printf( " -> %d\n", ircview->hl_e_pos );
					
					iv_render_hl = 0;
					ircview_update_font( ircview );
				}
				
				if ( return_ready == 1 )
					return 0;
				else if ( return_ready == 2 )
				{
					/* render everything until this point */
					if ( ircview_paintmode )
					{
						canvas_show_text( OBJECT(ircview), x+fxo, y, tmp_ptr, b );
						ircview_link_test( ircview, line, x+fxo, y, tmp_len, begin_pos );
					}
					
					/* and then bail out */
					*offset += last_print + b;
					
					return 0;
				}
				
				still_inc = 1; /* mark that we should keep incrementing last_print until we hit text */
				
				if ( ircview_paintmode )
				{
					canvas_show_text( OBJECT(ircview), x+fxo, y, tmp_ptr, b );
					ircview_link_test( ircview, line, x+fxo, y, tmp_len, begin_pos );
				}
				
				fxo += xo;
				
				tmp_len = 0;
			}
			
			b = ircview_do_fmt( fmt_ptr, line );
			
			/* offset extra for the formatting chars */
			iv_line_fmt_offset += b + 1;
			fmt_ptr += b + 1;
			o += b + 1;
		}
		
		if ( still_inc )
		{
			last_print = o;
			still_inc = 0;
		}
		
		if ( tmp_len == 0 )
			begin_pos = start_pos + a + 1;
		
		/* non-format char, save to a buffer */
		tmp[tmp_len] = *fmt_ptr; //cptr[a];
		tmp_len++;
		
		/* offset for the actual chars, too! */
		iv_line_fmt_offset++;
		fmt_ptr++;
		o++;
	}
	
	*offset = 0;
	
	return 0;
}

event_handler( ircview_handle_redraw )
{
	int scroll;
	canvas_widget_t *cvsw = (canvas_widget_t *)object;
	ircview_t *iv = (ircview_t *)object;
	node_t *n;
	
	int font_height = cvsw->widget.font.size;
	int padding = X_PADDING;
	int height = cvsw->widget.size_req->h;
	int usable_width = cvsw->widget.size_req->w - padding;
	int lines = ceil((float)height / font_height);
	int a, c;
	int is_hl = 0;
	int is_hls, is_hle, hln, hln2;
	int tmp_mouse_startx=0, tmp_mouse_starty=0;
	int tmp_mouse_endx=0, tmp_mouse_endy=0;

	canvas_fill_rect( OBJECT(iv), 0, 0, cvsw->widget.size_req->w, cvsw->widget.size_req->h, 1.0f, 1.0f, 1.0f, 1.0f );
	
	tmp_mouse_startx = mouse_startx;
	tmp_mouse_starty = mouse_starty;
	tmp_mouse_endx = mouse_endx;
	tmp_mouse_endy = mouse_endy;
	
	// forward or backward highlighting?
	if ( mouse_sel )
	{
		hln =  floor( (height-tmp_mouse_starty) / font_height );
		hln2 = floor( (height-tmp_mouse_endy) / font_height );
		
		if ( (hln == hln2 && tmp_mouse_startx > tmp_mouse_endx) || hln2 > hln )
		{
			tmp_mouse_startx = mouse_endx;
			tmp_mouse_starty = mouse_endy;
			tmp_mouse_endx = mouse_startx;
			tmp_mouse_endy = mouse_starty;
		}
	}
	
	scroll = ( LIST_LENGTH( &iv->lines ) - scrollbar_get_pos( iv->scroll ) + 1 );
	
	canvas_set_text_bgcolor( OBJECT(cvsw), 1, 1, 1, 1 );
	
	a = 0;
	c = 0;
	
	LIST_FOREACH_PREV( n, iv->lines.tail )
	{
		ircview_line_t *ivline = (ircview_line_t *)n->data;
		
		c++;
		
		ivline->visible = 0;
		
		if ( c+1 < scroll )
			continue;
		
		if ( a > lines )
			break;
		
		iv_render_hl = 0;
		iv_render_bold = 0;
		iv_render_italic = 0;
		iv_render_underline = 0;
		
		iv_line_fmt_offset = 0;
		
		
		char *tptr = ivline->text;
		int l = 0;
		int s;
		int os=0;
		
		ivline->visible = 1;
		ircview_paintmode = 0;
		
		while ( *tptr != 0 )
		{
			l++;
			
			ircview_show_text( iv, padding, 0, usable_width, &os, ivline,   0, 0, 0,   0, 0 );
			tptr = ivline->text + os;
			
			if ( os == 0 )
				break;
		}
		
		tptr = ivline->text;
		s = a + l;
		os = 0;
		
		ircview_paintmode = 1;

		strcpy( iv_render_fg, ircview_cache_cols[BTV_TimeStamp] );
		strcpy( iv_render_bg, ircview_cache_cols[BTV_Background] );

#define FIXNEG(a) ((a)<0?0:(a))
		while ( *tptr != 0 )
		{
			/* are we highlighting? */
			is_hls = is_hle = 0;
			
			if ( mouse_sel )
			{
				int tmpy = height - (s * font_height);
				int tmpy2 = tmpy+font_height;
				
				is_hls = (tmp_mouse_starty > tmpy) && (tmp_mouse_starty <= tmpy2);
				is_hle = (tmp_mouse_endy > tmpy) && (tmp_mouse_endy <= tmpy2);
				
				is_hl = (tmp_mouse_starty < tmpy) && (tmp_mouse_endy > tmpy2);
			}
			else
				is_hl = 0;
			
			if ( is_hls )
				iv->hl_s_line = ivline;
			
			if ( is_hle )
				iv->hl_e_line = ivline;
			
			/* display a single line of this "line" */
			ircview_show_text( iv, padding, (height) - (s * font_height), usable_width, &os, ivline,
				is_hl, is_hls, is_hle,
				FIXNEG((is_hls?tmp_mouse_startx:tmp_mouse_endx)-padding),
				(is_hle?FIXNEG(tmp_mouse_endx-padding):-1) );
			tptr = ivline->text + os;
			
			if ( os == 0 )
				break;
			
			s--;
			a++;
		}
		
		a++;
	}
}

event_handler( ircview_handle_scroll )
{
	node_t *n;
	ircview_t *iv;
	
	LIST_FOREACH( n, ircviews.head )
	{
		iv = (ircview_t *)n->data;
		if ( iv->scroll == object )
		{
			canvas_redraw( OBJECT(iv) );
			// FIXME: only do this if we're the active window, it's to effective
			// and causes cross-window focusing for every scroll event (new msg, too)
			//widget_focus( WIDGET(iv) ); // in turn, goes to input
		}
	}
}

void ircview_update_scroll( ircview_t *ircview )
{
	int a, b;
	canvas_widget_t *cvsw = (canvas_widget_t *)ircview;
	int height = cvsw->widget.size_req->h;
	int font_height = cvsw->widget.font.size;
	int lines = floor((float)height / font_height);
	
	if ( ircview->scroll == 0 )
		return;
	
	a = LIST_LENGTH(&ircview->lines) - 1;
	b = scrollbar_get_pos( ircview->scroll );
	
	if ( a < 0 )
		a = 0;
	
	// we're setting the maximum to include a whole extra page of scrolling
	scrollbar_set_range( ircview->scroll, 0, a+lines-1 );
	scrollbar_set_pagesize( ircview->scroll, lines );
	
	if ( a == 0 || a == b+1 )
	{
		scrollbar_set_pos( ircview->scroll, a );
	}
}

event_handler( ircview_handle_destroy )
{
	node_t *n;
	
	n = node_find( object, &ircviews );
	
	node_del( n, &ircviews );
	node_free( n );
}

ircview_t *ircview_widget_create( object_t *parent, bounds_t *b )
{
	object_t *c;
	ircview_t *iv;
	node_t *n;
	
	if ( ircview_cache_colours_extracted == 0 )
		ircview_cache_colours_update( );
	
	/* canvas/ircview creation */
	object_override_next_size( sizeof(ircview_t) );
	c = canvas_widget_create( parent, b, 0 );
	
	widget_set_notify( c, cNotifyMouse );
	
	/* save me */
	n = node_create( );
	node_add( c, n, &ircviews );
	
	/* ircview init */
	iv = (ircview_t *)c;
	list_create( &iv->lines );
	
	if ( line_heap == 0 )
	{
		/* create the line block heap */
		line_heap = block_heap_create( sizeof(ircview_line_t), 10, BH_NOW );
	}
	
	/* handlers */
	object_addhandler( c, "redraw", ircview_handle_redraw );
	object_addhandler( c, "clicked", ircview_handle_mousedown );
	object_addhandler( c, "released", ircview_handle_mouseup );
	object_addhandler( c, "mouse_moved", ircview_handle_mousemove );
	object_addhandler( c, "destroy", ircview_handle_destroy );
	object_addhandler( c, "right_released", ircview_handle_rightrel );
	
	/*c_btv_addline( c, " http://bersirc.free2code.net/index.php/home\\woo\\blah\\?what='the'&fcuk=\"this\"", 0, BTV_ClientWelcome );
	c_btv_addline( c, "We\037lcome to \037\035Bersirc 2.2\035 \037utilising\037 the Claro GUI Toolkit", 0, BTV_ClientWelcome );
	c_btv_addline( c, "Check out our website : http://bersirc.free2code.net/", 0, BTV_ClientWelcome );
	c_btv_addline( c, "Or take a look at Claro #claro : http://claro.free2code.net/", 0, BTV_ClientWelcome );
	c_btv_addline( c, "\0031L\0032o\0033r\0034e\0035m\0036 i\0037p\0038s\0039u\00310m \00311d\00312o\00313l\00314o\00315r s\00316i\00317t \002amet, \003consectetuer adipiscing elit. Pellentesque euismod feugiat dolor. Proin in sem. Sed ultricies malesuada ligula. Suspendisse potenti. Donec ornare ipsum non urna. Maecenas a elit non metus hendrerit laoreet. Sed feugiat condimentum arcu. Integer commodo accumsan sem. Integer ante mauris, mattis vitae, mattis eget, ullamcorper non, risus. Etiam nulla. Ut mollis magna nec lacus. Suspendisse potenti. Ut vulputate, arcu non tristique dignissim, urna lacus sodales sem, eu tristique enim arcu vel nulla. Integer interdum porta mi. Praesent tempor viverra justo. Class aptent taciti sociosqu ad litora torquent per conubianostra, per inceptos hymenaeos. Duis tempus neque ac ipsum. In ultrices pulvinar tellus. Fusce odio est, dapibus et, eleifend ut, tincidunt ut, lectus. Proin ultrices pellentesque est. Vivamus tincidunt, nulla nec posuere scelerisque, nunc quam dictum purus, eget molestie arcu arcu quis nisl. Nunc pharetra justo quis urna. Donec vel nibh ac ante hendrerit elementum. Aliquam sed ipsum ac ligula consequat facilisis. Sed quis tortor. Aenean nec nulla. Maecenas ac justo. Nam sit amet purus sed nibh posuere tempus. Vivamus quis risus eget velit suscipit consequat. Cras tortor elit, vehicula nec, tincidunt nec, aliquet eu, sapien. Aenean urna metus, congue quis, lobortis at, ultrices nec, ante. Aenean tellus urna, tempus et\002, fermentum id, sodales nec, dui.", 0, BTV_ClientWelcome );
	*/
	return (ircview_t *)c;
}

typedef struct
{
	char *prefix;
	
	int type;
} ircview_url_prefix_t;

ircview_url_prefix_t ircview_url_prefixes[] = {
	{ "http://", bIRCViewLinkTypeURL },
	{ "https://", bIRCViewLinkTypeURL },
	{ "ftp://", bIRCViewLinkTypeURL },
	{ "www.", bIRCViewLinkTypeURL },
	{ "news://", bIRCViewLinkTypeURL },
	{ "feed://", bIRCViewLinkTypeURL },
	{ "#", bIRCViewLinkTypeChannel },
	{ "&", bIRCViewLinkTypeChannel },
	{ NULL, 0 },
};

void ircview_line_update_links( ircview_line_t *line )
{
	int a, b, ll;
	
	if ( line->linkn != 0 )
		return;
	
	line->linkn = 0;
	list_create( &line->links );
	
	ll = strlen( line->text );
	
	for ( a = 0; a < ll; a++ )
	{
		char *link_tmp = &line->text[a];
		
		for ( b = 0; ircview_url_prefixes[b].prefix != NULL; b++ )
		{
			if ( !strncasecmp( ircview_url_prefixes[b].prefix, link_tmp, strlen( ircview_url_prefixes[b].prefix ) ) )
			{
				node_t *n;
				char *end_tmp = NULL, *tmp = NULL;
				int len;
				
				/* FIXME: block allocate this, they are reasonably common and the same size. */
				ircview_link_t *link = (ircview_link_t *)malloc( sizeof(ircview_link_t) );
				
				if ( link == NULL )
					return; /* allocation error */
				
				link->type = ircview_url_prefixes[b].type;
				
				/* found a match ! */
				
				//printf( "Found '%s'\n", ircview_url_prefixes[b].prefix );
				
				n = node_create( );
				node_add( link, n, &line->links );
				line->linkn++;
				
				end_tmp = strchr( link_tmp, ' ' );
				
				if ( end_tmp == NULL )
					len = strlen( link_tmp ); /* to end of string */
				else
					len = end_tmp - link_tmp; /* to space */
				
				tmp = (char *)malloc( len + 1 );
				link->link = (char *)malloc( len + 1 );
				strncpy( tmp, link_tmp, len );
				tmp[len] = 0;
				
				link->offset = a;
				link->length = len;
				
				irc_fmt_strip( link->link, tmp );
				
				free( tmp );
				
				//printf( "link = %s\n", link->link );
				
				a += len; /* skip this link, we don't want http://www. counting twice */
				break;
			}
		}
	}
}

void ircview_link_test( ircview_t *ircview, ircview_line_t *line, int x, int y, int chars, int start_offset )
{
	node_t *n;
	ircview_link_t *link;
	int xo = 0, width = 0;
	
	LIST_FOREACH( n, line->links.head )
	{
		link = (ircview_link_t *)n->data;
		
		if ( start_offset > link->offset )
			continue; /* past this link */
		
		if ( start_offset+chars < link->offset )
			continue; /* before this link */
		
		/* link starts in this test !! */
		//printf( "'%s' (%d)\n", line->text + start_offset, chars );
		/* X distance to the start of the link */
		xo = canvas_text_box_width( OBJECT(ircview), line->text + start_offset, link->offset - start_offset );
		
		/* width of the link */
		width = canvas_text_box_width( OBJECT(ircview), line->text + link->offset, strlen(link->link) );
		
		/* save position/size */
		link->x = x+xo;
		link->y = y;
		link->w = width;
		link->h = WIDGET(ircview)->font.size;
		
		//printf( "!! %d,%d,%d,%d\n", link->x, link->y, link->w, link->h );
	}
}

void ircview_update_line( ircview_line_t *line )
{
	int j;
	char time_fmt[128];
	char time[128];
	time_t curtime;
	
	j = strlen( line->original ) + 1;
	
	if ( line->text != 0 )
		free( line->text );
	
	if ( line->cleantext != 0 )
		free( line->cleantext );
	
	curtime = line->time;
	
	sprintf( time_fmt, "%s\003 ", b_get_option_string( xidentity, "time", "opt_time_line_timestamp" ) );
	strftime( time, 256, time_fmt, localtime( &curtime ) );
	
	j += strlen( time ) + 1;
	
	j += 10; /* safety padding for lazy colour checking */
	
	line->text = (char *)malloc( j );
	line->cleantext = (char *)malloc( j );
	
	memset( line->text, 0, j );
	memset( line->cleantext, 0, j );
	
	sprintf( line->text, "%s%s\003", time, line->original );
	
	irc_fmt_strip( line->cleantext, line->text );
	
	/* find links in the line */
	ircview_line_update_links( line );
}

void ircview_add_line( ircview_t *ircview, int colour, int flags, char *text )
{
	ircview_line_t *line;
	node_t *n;
	
	/* check line heap integrity */
	if ( line_heap == 0 )
		cassert( line_heap != 0, "ircview_add_line called, but line_heap not created!" );
	
	/* allocate a line */
	if ( ( line = block_alloc( line_heap ) ) == 0 )
	{
		clog( CL_ERROR, "Could not allocate a line from line_heap. This should never happen." );
		return;
	}
	
	/* prepare line data */
	line->original = strdup( text );
	line->text = 0;
	line->cleantext = 0;
	
	line->linkn = 0;
	
	line->time = time( 0 );
	line->colour = colour;
	line->flags = flags;
	
	ircview_update_line( line );
	
	/* append to ircview line list */
	n = node_create( );
	node_add( line, n, &ircview->lines );
	
	/* update scrolling */
	ircview_update_scroll( ircview );
	
	/* redraw the ircview */
	canvas_redraw( OBJECT(ircview) );
}

int ircview_printf( ircview_t *ircview, int colour, int flags, char *fmt, ... )
{
	va_list args;
	char buf[16384];
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	ircview_add_line( ircview, colour, flags, buf );
	
	return 1;
}

void ircview_set_scrollbar( ircview_t *ircview, object_t *s )
{
	ircview->scroll = s;
	
	object_addhandler( s, "scroll", ircview_handle_scroll );
	ircview_update_scroll( ircview );
}

void ircview_clear( ircview_t *ircview )
{
	node_t *n, *nn;
	ircview_line_t *line;
	
	LIST_FOREACH_SAFE( n, nn, ircview->lines.head )
	{
		line = (ircview_line_t *)n->data;
		
		if ( line->text )
			free( line->text );
		if ( line->cleantext )
			free( line->cleantext );
		if ( line->original )
			free( line->original );
		
		block_free( line_heap, line );
		
		node_del( n, &ircview->lines );
		node_free( n );
	}
	
	/* update scrolling */
	ircview_update_scroll( ircview );
	
	/* redraw the ircview */
	canvas_redraw( OBJECT(ircview) );
}

