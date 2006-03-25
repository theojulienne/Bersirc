/*************************************************************************
$Id: btextview.c 185 2005-11-03 02:14:13Z terminal $

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

#define DBG_BTV_PRINTF null_printf

void null_printf( char *a, ... )
{
	
}

#define CONTENT_INDENT 5
#define CONTENT_SPACING 0
#define CONTENT_RMARGIN 5
#define CONTENT_BMARGIN 5
#define LEFT_WRAP_MARGIN 0
#define PAGE_WIDTH ( bv->w->info->width - 20 - CONTENT_RMARGIN )

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

char *btv_colour_names[BTV_NUM_COLS] = {
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

char *cacheCols[BTV_NUM_COLS];

int btv_cache_colours_extracted = 0;

void btv_cache_colours_update( )
{
	int a;
	
	for ( a = 0; a < BTV_NUM_COLS; a++ )
	{
		cacheCols[a] = b_get_option_string( xidentity, "colours", btv_colour_names[a] );
	}

	btv_cache_colours_extracted = 1;
}

BTextView *btv_head = 0;
BTextView *btv_curr = 0;

BTextView *btv_by_widget( CWidget *w )
{
	BTextView *curr;
	
	for ( curr = btv_head; curr != 0; curr = curr->next )
	{
		if ( curr->w == w )
			return curr;
	}
	
	return NULL;
}

/** Painting functions **/

/* PORTHACK */
#if 0
typedef struct
{
	CWidget *w;
	char *text;
	int width;
} FontWidthCache;

FontWidthCache *fw_cache_list = 0;
int fw_cache_size = 0;

int cache_font_get_string_width( BTextView *w, char *text, int len ) {
	return c_font_get_string_width( w->w, text, len );
}

typedef struct
{
	CWidget *w;
	int height;
} FontHeightCache;

FontHeightCache *fh_cache_list = 0;
int fh_cache_size = 0;

int cache_font_get_height( CWidget *w ) {
	int a;
	
	for ( a = 0; a < fh_cache_size; a++ )
	{
		if ( fh_cache_list[a].w == w )
		{
			if ( fh_cache_list[a].height < 0 )
				fh_cache_list[a].height = c_font_get_height( w );
			
			return fh_cache_list[a].height;
		}
	}
	
	fh_cache_size++;
	
	fh_cache_list = (FontHeightCache *)realloc( fh_cache_list, fh_cache_size * sizeof(FontHeightCache) );
	
	fh_cache_list[a].w = w;
	fh_cache_list[a].height = c_font_get_height( w );
	
	return fh_cache_list[a].height;
}

#define B_PAINT_SIZEONLY 0x1

int realPaintText( BTextView *bv, char *txtin, int len, int x, int y, int w, char *fg, char *bg )
{
	char *txt;
	int a = 0;
	
	if ( len != strlen( txtin ) )
	{
		txt = (char *)malloc( len + 1 );
		strncpy( txt, txtin, len );
		txt[len] = 0;
		a = 1;
	} else
		txt = txtin;
	
	if ( w == -1 )
		w = cache_font_get_string_width( bv, txt, len );
	
	c_canvas_paint_fillrect( bv->w, c_make_color_hex( bg ), x, y+2, w, cache_font_get_height( bv->w ) );
	c_canvas_paint_text( bv->w, x, y, c_make_color_hex( fg ), txt );
	
	if ( a == 1 )
		free( txt );
	
	return 0;
}

int paintText( BTextView *bv, int linenum, int linepos, char *fulltxt, int len, int x, int y, char *inforeground, char *inbackground, int op )
{
	char *txt = (char *)malloc( len + 1 );
	int a, b, c;
	int hlx = x;
	char *hltxt;
	char fg[10], bg[10];
	int split_start_pos = 0, split_end_pos = len;
	int wordhili;
	
	int wh_s_line, wh_e_line;
	int wh_s_pos, wh_e_pos;
	
	strncpy( txt, fulltxt, len );
	txt[len] = 0;
	
	a = cache_font_get_string_width( bv, txt, strlen( txt ) );
	
	if ( op & B_PAINT_SIZEONLY )
	{
		free( txt );
		return a;
	}
	
	y -= cache_font_get_height( bv->w );
	
	strcpy( fg, inforeground );
	strcpy( bg, inbackground );
	
	if ( bv->hl_mode == 2 )
	{
		wh_s_line = bv->hl_s_line;
		wh_e_line = bv->hl_e_line;
		wh_s_pos  = bv->hl_s_pos;
		wh_e_pos  = bv->hl_e_pos;
		
		if ( ( wh_e_line < wh_s_line ) || ( wh_e_line == wh_s_line && wh_e_pos <= wh_s_pos ) )
		{
			goto paint_nohili_text;
		}
		
		// highlighting...
		if ( linenum >= wh_s_line && linenum <= wh_e_line )
		{
			// in a highlighted line
			
			wordhili = 0;
			
			if ( linenum == wh_s_line )
			{
				// is start cursor inside our word?
				if ( wh_s_pos <= linepos + len )
					wordhili = 1;
			}
			
			if ( linenum == wh_e_line )
			{
				// is end cursor before our word?
				if ( linepos <= wh_e_pos )
					wordhili = 1;
			}
			
			if ( linenum == wh_e_line && linenum == wh_s_line )
			{
				if ( linepos+len >= wh_s_pos && linepos <= wh_e_pos )
					wordhili = 1;
				else
					wordhili = 0;
			}
			
			// is this word highlighted at ALL?
			if ( wordhili || ( linenum > wh_s_line && linenum < wh_e_line ) )
			{
				//( linenum == wh_s_line && linepos > wh_s_pos-len ) || ( linenum == wh_e_line && linepos < wh_e_pos ) || 
				//split_start_pos = 0; split_end_pos = 0;
				
				// handle word split by highlight
				if ( linenum == wh_s_line && linepos < wh_s_pos && linepos > wh_s_pos-len )
				{
					// this word is split by the highlight start.
					split_start_pos = wh_s_pos - linepos;
				}
				
				if ( linenum == wh_e_line && linepos < wh_e_pos && linepos+len > wh_e_pos )
				{
					// this word is split by the highlight end
					split_end_pos = len - ( (linepos+len) - wh_e_pos ) + 1;
				}
				
				hltxt = txt;
				
				if ( split_start_pos > 0 )
				{
					// so, we were split at the start.
					b = cache_font_get_string_width( bv, hltxt, split_start_pos );
					realPaintText( bv, hltxt, split_start_pos, hlx, y, b, fg, bg );
					hlx += b;
					hltxt += split_start_pos;
				}
				
				c = split_end_pos - split_start_pos;
				if ( c > 0 )
				{
					// we have something to display highlighted
					b = cache_font_get_string_width( bv, hltxt, c );
					realPaintText( bv, hltxt, c, hlx, y, b, cacheCols[BTV_HighlightText], cacheCols[BTV_Highlight] );
					hlx += b;
					hltxt += c;
				}
				
				c = len-split_end_pos;
				if ( c > 0 )
				{
					// so, we were split at the end.
					b = cache_font_get_string_width( bv, hltxt, c );
					realPaintText( bv, hltxt, c, hlx, y, b, fg, bg );
					hlx += b;
					hltxt += c;
				}
				
				goto paint_text_display_end;
			}
			/*strcpy( fg, cacheCols[BTV_HighlightText] );
			strcpy( bg, cacheCols[BTV_Highlight] );*/
		}
	}
	
	paint_nohili_text:
	realPaintText( bv, txt, strlen( txt ), x, y, a, fg, bg );
	
	paint_text_display_end:
	free( txt );
	return a;
}

void c_btv_highlight_reset( BTextView *btv )
{
	btv->hl_mode = 0;
	btv->hl_s_line = 0;
	btv->hl_s_pos = 0;
	btv->hl_e_line = 0;
	btv->hl_e_pos = 0;
}

void c_btv_handle_mouse_wheel( object_t *obj, event_t *e )
{
	int *dat = (int *)data;
	//int x = dat[0];
	//int y = dat[1];
	int d = dat[2];
	BTextView *bw;
	BServerWindow *win;
	
	if ( ( bw = btv_by_widget( e->caller ) ) == 0 )
	{
		if ( ( win = b_find_any_by_widget( e->caller ) ) == 0 )
			return;
			
		bw = win->content;
	}
	
	if ( bw == 0 )
		return;
	
	c_scroll_by( bw->w, C_SCROLLING_VERT, -(d/100) );
}

void c_btv_handle_mouse_left_down( object_t *obj, event_t *e )
{
	int *dat = (int *)data;
	int x = dat[0];
	int y = dat[1];
	BTextView *btv;
	BServerWindow *sw;
	BChatWindow *cw;
	
	btv = btv_by_widget( e->caller );
	
	btv->hl_mode = 1; // started.
	btv->hl_msx = btv->hl_mx = x;
	btv->hl_msy = btv->hl_my = y;
	
	sw = b_find_server_by_widget( e->caller );
	
	if ( sw )
		c_widget_focus( sw->input );
	else
	{
		cw = b_find_chat_by_widget( e->caller );
		
		if ( cw )
			c_widget_focus( cw->input );
	}
}
#endif
char *btv_make_line( BTextView *bv, BTVLine *line )
{
	int j;
	char *txt;
	char time_fmt[128];
	time_t curtime;
	
	j = strlen( line->text ) + 1;
	txt = (char *)malloc( j + 32 + 256 );
	
	curtime = line->time;
	sprintf( time_fmt, "%s\003 ", b_get_option_string( xidentity, "time", "opt_time_line_timestamp" ) );
	strftime( txt, 256, time_fmt, localtime( &curtime ) );

	strcat( txt, line->text );
	strcat( txt, "\003" );
	
	return txt;
}
/* PORTHACK */
#if 0
void c_btv_handle_mouse_right_release( object_t *obj, event_t *e )
{
	int *dat = (int *)data;
	int x = dat[0];
	int y = dat[1];
	BServerWindow *sw;
	BChatWindow *cw;
	CWidget *popup;
	
	sw = 0;
	cw = 0;
	
	if ( ( sw = b_find_server_by_widget( e->caller ) ) == NULL )
	{
		if ( ( cw = b_find_chat_by_widget( e->caller ) ) == NULL )
			return;
	}
	
	popup = 0;
	
	if ( sw != 0 )
		popup = sw->conmenu.menu;
	else if ( cw != 0 )
		popup = cw->conmenu.menu;
	
	if ( popup != 0 )
	{
		x += c_widget_screen_offset_x( e->caller );
		y += c_widget_screen_offset_y( e->caller );
		c_menubar_popup( popup, x, y );
	}
}

void c_btv_handle_mouse_left_release( object_t *obj, event_t *e )
{
	BTextView *btv;
	btv_linkarea *curr;
	BTVLine *line;
	char *text, *tmp, *tmpo;
	int a, b;
	int *dat = (int *)data;
	int x = dat[0];
	int y = dat[1];
	
	btv = btv_by_widget( e->caller );
	
	if ( btv->hl_mode == 2 )
	{
		if ( ( btv->hl_e_line < btv->hl_s_line ) || ( btv->hl_e_line == btv->hl_s_line && btv->hl_e_pos <= btv->hl_s_pos ) )
		{
			// invalid
		}
		else
		{
			a = 0;
			text = 0;
			
			for ( b = btv->hl_s_line; b <= btv->hl_e_line; b++ )
			{
				line = &btv->lines_data[b];
				tmp = tmpo = btv_make_line( btv, line );
				a += strlen( tmp ) + 3;
				text = (char *)realloc( text, a );
				
				if ( b == btv->hl_e_line )
				{
					tmp[btv->hl_e_pos+1] = 0;
				}
				
				if ( b == btv->hl_s_line )
				{
					strcpy( text, "" );
					tmp += btv->hl_s_pos;
				}
				
				strcat( text, tmp );
#ifdef ENV_WIN32
				strcat( text, "\r\n" );
#else
				strcat( text, "\n" );
#endif
				free( tmpo );
			}
			
			if ( text != 0 )
			{
				c_clipboard_set_text( btv->w, text );
				free( text );
			}
		}
	}
	
	btv->hl_mode = 0;
	btv->hl_s_line = btv->hl_e_line = 0;
	btv->hl_s_pos = btv->hl_s_pos = 0;
	
	for ( curr = btv->links; curr != 0; curr = curr->next )
	{
		if ( curr->x1 < x && x < curr->x2 &&
		     curr->y1 < y && y < curr->y2 )
		{
			c_open_browser( e->caller, curr->url );
			return;
		}
	}
	
	c_send_event( e->caller, C_EVENT_UPDATE, 0 );
}

void c_btv_handle_mouse_move( object_t *obj, event_t *e )
{
	int *dat = (int *)data;
	int x = dat[0];
	int y = dat[1];
	BTextView *btv;
	btv_linkarea *curr;
	
	btv = btv_by_widget( e->caller );
	
	if ( btv->hl_mode > 0 ) {
		btv->hl_mx = x;
		btv->hl_my = y;
	}
	
	for ( curr = btv->links; curr != 0; curr = curr->next )
	{
		if ( curr->x1 < x && x < curr->x2 &&
		     curr->y1 < y && y < curr->y2 )
		{
#ifdef ENV_WIN32
			// FIXME
			SetCursor( LoadCursor( NULL, IDC_HAND ) );
#endif
			return;
		}
	}
	
	if ( btv->hl_mode > 0 )
		c_send_event( e->caller, C_EVENT_UPDATE, 0 );
}

int btv_find_hl_charpos( BTextView *bv, char *txt, int spos, int slen, int x, int mx )
{
	char *word = (char *)malloc( slen + 1 );
	int a=spos, b, c;
	
	strncpy( word, &txt[spos], slen + 1 );
	word[slen] = 0;
	
	for ( b = 1; b < slen; b++ )
	{
		c = cache_font_get_string_width( bv, word, b );
		
		if ( x + c <= mx )
			a = spos + b;
	}
	
	free( word );
	
	return a;
}

void paintLine( BTextView *bv, int linenum, int inx, int iny, int op )
{
	int a, b, c;
	int i, j, k;
	int linepos;
//	int f, b;
	int x, y;
	char *txtbak, *txt;
	BTVLine *line;
	
	btv_linkarea *link;
	
	int urlc;
	int urllens[] = { 7, 6, 8, 4, 7, 0 };
	char *urlstarts[] = {
		"http://",
		"ftp://",
		"https://",
		"www.",
		"news://",
		0,
	};
	
	char currbg[8];
	char currfg[8];
	
	// vars for loop
	int m, n, o, ip;
	//int ln = 0;
	char *cr;
	int spaceleft;
	int lx = 0;
	int font_height = 0;
	
	//DBG_BTV_PRINTF( "Welcome to BTV line painter version 1.0! ;-)\n" );
	
	if ( linenum >= bv->lines_num )
		return;
	
	line = &bv->lines_data[linenum];

	line->validated = 1;

	txtbak = txt = btv_make_line( bv, line );

	j = strlen( txt );
	
	//DBG_BTV_PRINTF( "Ready for colouring.\n" );
	
	strcpy( currfg, cacheCols[BTV_TimeStamp] );
	strcpy( currbg, cacheCols[BTV_Background] );

	//DBG_BTV_PRINTF( "Colours set.\n" );

	font_height = cache_font_get_height( bv->w );

	x = 0;
	y = 1;

	//DBG_BTV_PRINTF( "Going into loop! Wish me luck!\n" );

	/*
	Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Pellentesque euismod feugiat dolor. Proin in sem. 
	Sed ultricies malesuada ligula. Suspendisse potenti. Donec ornare ipsum non urna. Maecenas a elit non metus 
	hendrerit laoreet. Sed feugiat condimentum arcu. Integer commodo accumsan sem. Integer ante mauris, mattis 
	vitae, mattis eget, ullamcorper non, risus. Etiam nulla. Ut mollis magna nec lacus. Suspendisse potenti. Ut 
	vulputate, arcu non tristique dignissim, urna lacus sodales sem, eu tristique enim arcu vel nulla. Integer 
	interdum porta mi. Praesent tempor viverra justo. Class aptent taciti sociosqu ad litora torquent per conubia
	nostra, per inceptos hymenaeos. Duis tempus neque ac ipsum. In ultrices pulvinar tellus. Fusce odio est, 
	dapibus et, eleifend ut, tincidunt ut, lectus. Proin ultrices pellentesque est. Vivamus tincidunt, nulla nec 
	posuere scelerisque, nunc quam dictum purus, eget molestie arcu arcu quis nisl. Nunc pharetra justo quis urna. 
	Donec vel nibh ac ante hendrerit elementum. Aliquam sed ipsum ac ligula consequat facilisis. Sed quis tortor. 
	Aenean nec nulla. Maecenas ac justo. Nam sit amet purus sed nibh posuere tempus. Vivamus quis risus eget velit 
	suscipit consequat. Cras tortor elit, vehicula nec, tincidunt nec, aliquet eu, sapien. Aenean urna metus, 
	congue quis, lobortis at, ultrices nec, ante. Aenean tellus urna, tempus et, fermentum id, sodales nec, dui.
	*/
	
	c_font_set_bold( bv->w->font, false );
	c_font_set_italic( bv->w->font, false );
	c_font_set_underline( bv->w->font, false );

	for ( i = k = 0; i < j; i++ ) {
		// for a change in formatting of any kind or spaces (for wrapping),
		// we output everything up until here in our old format.
		
		if ( k > lx )
			lx = k;
		
		linepos = lx;

		//DBG_BTV_PRINTF( "Checking for special-char. If so, formatting etc.\n" );

		if ( txt[i] == '\002' || txt[i] == '\037' || txt[i] == '\035' || txt[i] == '\003' || txt[i] == 15 || txt[i] == '\004' || txt[i] == '\17' || txt[i] == '\26' || txt[i] == ' ' ) {
			a = 0;
			
			n = cache_font_get_string_width( bv, txt + lx, i - lx );
			cr = txt + lx;
			
			if ( !( op & B_PAINT_SIZEONLY ) ) {
				for ( c = 0; c < i - lx; c++ )
				{
					if ( txt[lx+c+1] == '\'' || txt[lx+c+1] == '"' )
						continue;
					
					break;
				}
				
				for ( urlc = 0; urlstarts[urlc] != 0; urlc++ )
				{
					if ( i - lx > c+urllens[urlc] )
					{
						a = 1; // say it is...
						
						for ( b = 0; b < urllens[urlc]; b++ )
						{
							if ( urlstarts[urlc][b] != txt[lx+c+b+1] )
							{
								// it isn't.
								a = 0;
								break;
							}
						}
						
						if ( a == 1 )
							break;
					}
				}
				
				if ( a == 1 )
				{
					// It's a URL. Save some info here...
					
					link = (btv_linkarea *)malloc( sizeof(btv_linkarea) );
					link->next = bv->links;
					bv->links = link;
					
					link->x1 = x;
					link->x2 = x + n;
					link->y1 = iny + (y-1) * font_height;
					link->y2 = iny + y * font_height;
					
					link->url = (char *)malloc( i - lx + 1 ); // don't need + 1 but it doesn't hurt.
					
					for ( b = i - lx - 1; b > 0; b-- )
					{
						if ( txt[lx+b] == '\'' || txt[lx+b] == '"' )
							continue;
						
						break;
					}
					
					strncpy( link->url, txt+lx+1+c, b-c );
					link->url[b-c] = 0;
				}
				
				// highlighting
				if ( bv->hl_mode > 0 )
				{
					// work out some values for later
					int bx1, by1, bx2, by2;
					
					bx1 = x;
					bx2 = x + n;
					by1 = iny + (y-1) * font_height;
					by2 = iny + y * font_height;

					if ( bv->hl_mode == 1 )
					{
						// in pre-highlight mode.
						
						if ( bx1 < bv->hl_msx && bv->hl_msx < bx2 &&
			     			 by1 < bv->hl_msy && bv->hl_msy < by2 )
						{
							// this segment is where highlighting started!
							bv->hl_s_line = linenum;
							bv->hl_s_pos = lx;
							
							// find start char
							bv->hl_s_pos = btv_find_hl_charpos( bv, txt, lx, i - lx, x, bv->hl_msx );
														
							// now we're in highlight mode, because we know our origin.
							bv->hl_mode = 2;
						}
					}
					
					if ( bv->hl_mode == 2 )
					{
						// in real highlighting
						
						if ( bx1 < bv->hl_mx && //bv->hl_mx < bx2 &&
			     			 by1 < bv->hl_my && bv->hl_my < by2 )
						{
							// this segment is where highlighting ends at the moment!
							bv->hl_e_line = linenum;
							bv->hl_e_pos = lx;
							
							// find end char
							bv->hl_e_pos = btv_find_hl_charpos( bv, txt, lx, i - lx, x, bv->hl_mx );
							
							//printf( "%d,%d -> %d,%d\n", bv->hl_s_line, bv->hl_s_pos, bv->hl_e_line, bv->hl_e_pos );
						}
					}
				}
			}

			if ( n > PAGE_WIDTH-LEFT_WRAP_MARGIN ) {
				// split
				spaceleft = (PAGE_WIDTH-LEFT_WRAP_MARGIN) - x;
				ip = i - lx;
				for ( ; n > 0; ) {
					o = -1;
					for ( m = 0; m < ip; m++ ) {
						// can we still fit it?
						if ( cache_font_get_string_width( bv, cr, m ) > spaceleft ) {
							o = m - 1; // no! that's the max!
							break;
						}
						// we can still fit, so keep going!
					}

					// if we're here, then we're ready to output some txtz0r!
					if ( o < 0 ) {
						// this is the end of the text, and it fits.
						n = 0;
						x += paintText( bv, linenum, linepos, cr, ip, x, iny + y * font_height, currfg, currbg, op );
					} else {
						// some more output, not getting to the end yet.
						x += paintText( bv, linenum, linepos, cr, o, x, iny + y * font_height, currfg, currbg, op );
						y++;
						n -= cache_font_get_string_width( bv, cr, o );
						x = LEFT_WRAP_MARGIN;
						cr += o;
						ip -= o;

						spaceleft = (PAGE_WIDTH-LEFT_WRAP_MARGIN) - x;
					}
				}
			} else {
				// one line.
				if ( x + n > PAGE_WIDTH-LEFT_WRAP_MARGIN ) {
					// needs to go onto second line
					y++;
					x = LEFT_WRAP_MARGIN;
					if ( a == 1 )
					{
						// fixey fixey
						link->y1 += font_height;
						link->y2 += font_height;
						link->x1 = x;
						link->x2 = x + n;
					}
					
					x += paintText( bv, linenum, linepos, txt + lx, i - lx, x, iny + y * font_height, currfg, currbg, op );
				} else {
					// same line.
					x += paintText( bv, linenum, linepos, txt + lx, i - lx, x, iny + y * font_height, currfg, currbg, op );
				}
			}

			lx = i;
	
			// flick bold
			if ( txt[i] == '\002' ) {
				if ( c_font_is_bold( bv->w->font ) )
					c_font_set_bold( bv->w->font, false );
				else
					c_font_set_bold( bv->w->font, true );
					
				lx++;
			}
	
			// flick underline
			if ( txt[i] == '\037' ) {
				if ( c_font_is_underline( bv->w->font ) )
					c_font_set_underline( bv->w->font, false );
				else
					c_font_set_underline( bv->w->font, true );
				lx++;
			}
	
			// flick italic
			if ( txt[i] == '\035' ) {
				if ( c_font_is_italic( bv->w->font ) )
					c_font_set_italic( bv->w->font, false );
				else
					c_font_set_italic( bv->w->font, true );
				lx++;
			}
	
			// reverse colours
			if ( txt[i] == '\026' ) {
				char tmpcol[8];
				strcpy( tmpcol, currfg );
				strcpy( currfg, currbg );
				strcpy( currbg, tmpcol );
				lx++;
			}
			
			// reset colours
			if ( txt[i] == 15 ) {
				strcpy( currfg, cacheCols[line->colour] );
				strcpy( currbg, cacheCols[BTV_Background] );
				
				c_font_set_italic( bv->w->font, false );
				c_font_set_underline( bv->w->font, false );
				c_font_set_bold( bv->w->font, false );
				
				lx++;
			}
	
			// flick ( Hex ) colours
			if ( txt[i] == '\004' ) {
				int p, z;
				for ( p = 1, z = 1; z <= 6; z++ ) {
					if ( !( ( txt[i+z] >= '0' && txt[i+z] <= '9' ) || ( txt[i+z] >= 'A' && txt[i+z] <= 'F' ) ) ) {
						p = -1;
					}
				}
	
				k = i + 1;
				lx++;
	
				if ( p == -1 ) {
					// not valid hex.
	
					// not sure about this. reset colours?
					strcpy( currfg, cacheCols[line->colour] );
					strcpy( currbg, cacheCols[BTV_Background] );
				} else {
					// moo. okay.
					currfg[0] = '#';
					for ( z = 1; z <= 6; z++ )
						currfg[z] = txt[i+z];
					currfg[z] = 0;
					i += 6;
					k = i + 1;
	
					lx += 6;
	
					i++;
					if ( txt[i] == ',' ) {
						for ( p = 1, z = 1; z <= 6; z++ ) {
							if ( !( ( txt[i+z] >= '0' && txt[i+z] <= '9' ) || ( txt[i+z] >= 'A' && txt[i+z] <= 'F' ) ) ) {
								p = -1;
							}
						}
	
						if ( p == -1 ) {
							// not valid. ignore
						} else {
							// okay!!
							currbg[0] = '#';
							for ( z = 1; z <= 6; z++ )
								currbg[z] = txt[i+z];
							currbg[z] = 0;
							i += 6;
							k = i + 1;
							lx += 7;
						}
					}
					i--;
				}
			}
	
			// flick colours
			if ( txt[i] == '\003' ) {
				int col = -2;
	
				if ( txt[i+1] == '0' ) {
					// this could be a 0-padded number. is it?
					if ( txt[i+2] >= '0' && txt[i+2] <= '9' ) {
						// we'll treat this as a 2-digit 0-padded number.
						col = txt[i+2] - '0';
						i += 2;
					} else {
						// they meant colour 0.
						col = 0;
						i += 1;
					}
				} else if ( txt[i+1] == '1' ) {
					// this could be a number > 9. is it?
					if ( txt[i+2] >= '0' && txt[i+2] <= '5' ) {
						// we'll treat this as a 2-digit number between 9 and 15.
						col = 10 + txt[i+2] - '0';
						i += 2;
					} else {
						// they meant colour 1.
						col = 1;
						i += 1;
					}
				} else if ( txt[i+1] == '9' && txt[i+2] == '9' ) {
					col = -1;
					i += 2;
				} else if ( txt[i+1] >= '2' && txt[i+1] <= '9' ) {
					// this is a normal number, that CANNOT have a second digit.
					col = txt[i+1] - '0';
					i += 1;
				} else {
					// this is a colour reset, as some clients
					// treat an empty colour number as such.
					col = -1;
				}
	
				k = i + 1;
				lx = i + 1;
	
				if ( col == -1 ) {
					strcpy( currfg, cacheCols[line->colour] );
					strcpy( currbg, cacheCols[BTV_Background] );
					continue; // we don't want to check for setting of background :)
				} else {
					strcpy( currfg, irccolours[col] );
				}
	
				i++;
				if ( txt[i] == ',' ) {
					// background too!
	
					col = -2;
					
					if ( txt[i+1] == '0' ) {
						// this could be a 0-padded number. is it?
						if ( txt[i+2] >= '0' && txt[i+2] <= '9' ) {
							// we'll treat this as a 2-digit 0-padded number.
							col = txt[i+2] - '0';
							i += 2;
						} else {
							// they meant colour 0.
		 					col = 0;
							i += 1;
						}
					} else if ( txt[i+1] == '1' ) {
						// this could be a number > 9. is it?
						if ( txt[i+2] >= '0' && txt[i+2] <= '5' ) {
							// we'll treat this as a 2-digit number between 9 and 15.
							col = 10 + txt[i+2] - '0';
							i += 2;
						} else {
							// they meant colour 1.
							col = 1;
							i += 1;
						}
					} else if ( txt[i+1] >= '2' && txt[i+1] <= '9' ) {
						// this is a normal number, that CANNOT have a second digit.
						col = txt[i+1] - '0';
						i += 1;
					} else {
						// eek? :)
						i--;
						continue;
					}
					if ( col != -1 ) {
						strcpy( currbg, irccolours[col] );
					}
					k = i + 1;
					lx = i + 1;
					// :)
				}
				i--;
			}
		}
	}
	
	c_font_set_bold( bv->w->font, false );
	c_font_set_italic( bv->w->font, false );
	c_font_set_underline( bv->w->font, false );

	//DBG_BTV_PRINTF( "I did the loop! Freeing temporary text.\n" );

	free( txtbak );

	//p.end( );

	//DBG_BTV_PRINTF( "Saving line height.\n" );

	line->height = y * font_height;

	//line->pm->resize( width(), line->height );
	
	//DBG_BTV_PRINTF( "Bye ;) [Font height : %d * lines (%d) = %d]\n", font_height, y, line->height );
}

int updateLines( BTextView *btvw, int start, int len )
{
	// repaints len lines starting from start

	int a, b, c, d;
	
	//DBG_BTV_PRINTF( "All lines set to 'not shown'\n" );

	b = btvw->w->info->height - CONTENT_BMARGIN;
	
	//DBG_BTV_PRINTF( "Got 'b' height stuff\n" );

	for ( a = len - 1; a >= 0; a-- ) {
		
		if ( start + a < 0 )
			continue;
		
		c = start + a;
		
		paintLine( btvw, c, 0, 0, B_PAINT_SIZEONLY );
		
		if ( c >= btvw->lines_num || c < 0 )
			continue;
		
		d = btvw->lines_data[c].height;
		
		b -= d;
		
		paintLine( btvw, c, 0, b, 0 );
	}
	
	return 0;
}

void displayCurrentPage( BTextView *btvw ) {
	btv_linkarea *curr, *next;
	int max, ps;
	int a, b, c, d;
	
	// clear all current links
	if ( btvw->links != 0 )
	{
		curr = btvw->links;
		while ( curr != 0 )
		{
			next = curr->next;
			
			free( curr->url );
			free( curr );
			
			curr = next;
		}
	}
	
	btvw->links = 0;

	//DBG_BTV_PRINTF( "displayCurrentPage( )\n" );

	c_canvas_paint_clear( btvw->w, c_make_color_hex( cacheCols[BTV_Background] ) );
	
	//DBG_BTV_PRINTF( "Cleared canvas.\n" );

	b = cache_font_get_height( btvw->w );
	a = btvw->lines_num * b;
	
	if ( btvw->lines_num == 0 )
		return;

	if ( a > btvw->w->info->height ) {
		c = ceil( btvw->w->info->height / b ) + 1;
	} else {
		// they all fit :) no scrolling happens.
		c = btvw->lines_num;
	}
	
	ps = floor( btvw->w->info->height / b );
	max = btvw->lines_num + ps - 1 - 1;
	
	c_set_scroll( btvw->w, C_SCROLLING_VERT, 0, max, ps - 1 );
	
	//printf( "fh=%d,lin=%d\n", b, c_get_scroll_pos( btvw->w, C_SCROLLING_VERT ) );
	
	d = btvw->lines_num - c - ( btvw->lines_num - c_get_scroll_pos( btvw->w, C_SCROLLING_VERT ) );

	// c is the number of lines to render.
	// d is the number in the array we start from.

	//DBG_BTV_PRINTF( "Updating lines.\n" );
	
	updateLines( btvw, d, c );
}
#endif
/** Claro Callbacks **/
void c_btv_handle_draw( object_t *obj, event_t *e )
{
	//displayCurrentPage( btv_by_widget( e->caller ) );
}

void c_btv_handle_resize( object_t *obj, event_t *e )
{
	//DBG_BTV_PRINTF( "c_btv_handle_resize( )\n" );
}

void c_btv_handle_destroy( object_t *obj, event_t *e )
{
	BTextView *bv = btv_by_widget( obj );
	
	c_btv_clear( bv );
	
	// patch
	if ( bv->prev )
		bv->prev->next = bv->next;
	
	if ( bv->next )
		bv->next->prev = bv->prev;
	
	// check for curr/head
	if ( btv_head == bv )
		btv_head = bv->next;
	
	if ( btv_curr == bv )
		btv_curr = bv->prev;
	
	free( bv );
	
	printf( "BTV has been destroyed!\n" );
	//DBG_BTV_PRINTF( "c_btv_handle_destroy( )\n" );
}

/** Editing Functions **/

int c_btv_printf( BTextView *btv, int flags, int col, char *fmt, ... )
{
	va_list args;
	char buf[16384];
	
	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );
	
	c_btv_addline( btv, buf, flags, col );
	
	return 1;
}

void c_btv_addline( BTextView *btv, char *line, int flags, int col )
{
	int a = btv->lines_num;
	int b;
	int textsize = btv->lines_size;
	BTVLine *lines = btv->lines_data;
	BTVLine *newlines;
	BTVLine *nline;
	
	a++;
	
	newlines = (BTVLine *)realloc( lines, sizeof(BTVLine) * a );
	
	nline = &newlines[a-1];
	
	b = strlen( line );
	
	nline->text = (char *)malloc( b + 1 );
	textsize += b;
	strncpy( nline->text, line, b+1 );
	nline->flags = flags;
	nline->colour = col;
	nline->time = time( 0 );
	nline->validated = 0;
	
	btv->lines_data = newlines;
	btv->lines_size = textsize;
	btv->lines_num = a;
	
	//DBG_BTV_PRINTF( "c_btv_addline( )\n" );
	
	c_set_scroll( btv->w, C_SCROLLING_VERT, 0, a, 1 );
	
	// keep to bottom unless we've been manually moved up
	if ( a == 1 )
		c_set_scroll_pos( btv->w, C_SCROLLING_VERT, 1 );
	else
	{
		b = c_get_scroll_pos( btv->w, C_SCROLLING_VERT );
		
		if ( b >= a - 1 )
		{
			c_set_scroll_pos( btv->w, C_SCROLLING_VERT, a );
		}
	}
	
	c_canvas_invalid( btv->w );
	c_send_event_singular( btv->w, C_EVENT_PREDRAW, 0 );
}

void c_btv_clear( BTextView *btv )
{
	int a = btv->lines_num;
	int b;
	BTVLine *lines = btv->lines_data;
	
	if ( a == 0 )
		return;
	
	for ( b = 0; b < a; b++ )
		free( lines[b].text );
	
	free( lines );

	btv->lines_data = 0;
	btv->lines_size = 0;
	btv->lines_num = 0;
	
	c_set_scroll( btv->w, C_SCROLLING_VERT, 0, 0, 1 );
	
	c_canvas_invalid( btv->w );
	c_send_event_singular( btv->w, C_EVENT_PREDRAW, 0 );
}

BTVLine *c_btv_getline( BTextView *btv, int l )
{
	int a = btv->lines_num;
	BTVLine *lines = btv->lines_data;
	
	if ( l < a && l >= 0 )
		return &lines[l];
	else
		return 0;
}

CWidget *c_btv( CWidget *parent, int x, int y, int width, int height, int flags )
{
	CWidget *w;
	BTextView *bw;
	CFont *f;
	
	if ( btv_cache_colours_extracted == 0 )
	{
		btv_cache_colours_update( );
	}
	
	//DBG_BTV_PRINTF( "Creating new c_canvas...\n" );
	/* PORTHACK */
#if 0
	w = c_new_canvas( parent, x, y, width, height, C_SCROLLING_VERT | C_BORDER_NONE | C_SCROLLING_GREY_NOT_HIDE );
	
	f = c_font_clone( w->font );
	c_font_bind_widget( f, w );
	
	//DBG_BTV_PRINTF( "Assigning draw handler...\n" );
	
	c_new_event_handler( w, C_EVENT_DESTROY, c_btv_handle_destroy );

	c_new_event_handler( w, C_EVENT_DRAW, c_btv_handle_draw );
	
	c_new_event_handler( w, C_EVENT_MOUSE_MOVE, c_btv_handle_mouse_move );
	c_new_event_handler( w, C_EVENT_MOUSE_LEFT_RELEASE, c_btv_handle_mouse_left_release );
	c_new_event_handler( w, C_EVENT_MOUSE_LEFT_CLICK, c_btv_handle_mouse_left_down );
	c_new_event_handler( w, C_EVENT_MOUSE_RIGHT_RELEASE, c_btv_handle_mouse_right_release );
	c_new_event_handler( w, C_EVENT_MOUSE_WHEEL, c_btv_handle_mouse_wheel );
#endif
	//DBG_BTV_PRINTF( "Creating a BTextView structure for data...\n" );
	
	bw = (BTextView *)malloc( sizeof( BTextView ) );
	
	//DBG_BTV_PRINTF( "Setting some ...\n" );
	
	memset( bw, 0, sizeof( BTextView ) );
	
	bw->next = 0;
	bw->links = 0;
	
	if ( btv_curr == 0 ) {
		btv_curr = btv_head = bw;
		bw->prev = 0;
	} else {
		btv_curr->next = bw;
		bw->prev = btv_curr;
		btv_curr = bw;
	}
	
	bw->w = w;
	bw->lines_data = 0;
	bw->lines_size = 0;
	bw->lines_num = 0;
	
#if 0 /* PORTHACK */
	c_btv_highlight_reset( bw );
#endif
	
	//DBG_BTV_PRINTF( "Adding lines to BTextView...\n" );
	
	//c_btv_addline( bw, "Welcome to \002Bersirc 2.2\002!", 0, BTV_ClientWelcome );
	
	/*
	c_btv_addline( bw, "We\037lcome to \037\035Bersirc 2.2\035 \037utilising\037 the Claro GUI Toolkit", 0, BTV_ClientWelcome );
	c_btv_addline( bw, "Check out our website : http://bersirc.free2code.net/", 0, BTV_ClientWelcome );
	c_btv_addline( bw, "Or take a look at Claro : http://claro.free2code.net/", 0, BTV_ClientWelcome );
	c_btv_addline( bw, "\0031L\0032o\0033r\0034e\0035m\0036 i\0037p\0038s\0039u\00310m \00311d\00312o\00313l\00314o\00315r s\00316i\00317t \002amet, \003consectetuer adipiscing elit. Pellentesque euismod feugiat dolor. Proin in sem. Sed ultricies malesuada ligula. Suspendisse potenti. Donec ornare ipsum non urna. Maecenas a elit non metus hendrerit laoreet. Sed feugiat condimentum arcu. Integer commodo accumsan sem. Integer ante mauris, mattis vitae, mattis eget, ullamcorper non, risus. Etiam nulla. Ut mollis magna nec lacus. Suspendisse potenti. Ut vulputate, arcu non tristique dignissim, urna lacus sodales sem, eu tristique enim arcu vel nulla. Integer interdum porta mi. Praesent tempor viverra justo. Class aptent taciti sociosqu ad litora torquent per conubianostra, per inceptos hymenaeos. Duis tempus neque ac ipsum. In ultrices pulvinar tellus. Fusce odio est, dapibus et, eleifend ut, tincidunt ut, lectus. Proin ultrices pellentesque est. Vivamus tincidunt, nulla nec posuere scelerisque, nunc quam dictum purus, eget molestie arcu arcu quis nisl. Nunc pharetra justo quis urna. Donec vel nibh ac ante hendrerit elementum. Aliquam sed ipsum ac ligula consequat facilisis. Sed quis tortor. Aenean nec nulla. Maecenas ac justo. Nam sit amet purus sed nibh posuere tempus. Vivamus quis risus eget velit suscipit consequat. Cras tortor elit, vehicula nec, tincidunt nec, aliquet eu, sapien. Aenean urna metus, congue quis, lobortis at, ultrices nec, ante. Aenean tellus urna, tempus et\002, fermentum id, sodales nec, dui.", 0, BTV_ClientWelcome );
	*/
	
	return w;
}

//
