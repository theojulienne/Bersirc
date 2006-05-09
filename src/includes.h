/*************************************************************************
$Id: includes.h 150 2005-08-10 01:03:47Z terminal $

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

#ifndef BERSIRC_INCLUDES_H
#define BERSIRC_INCLUDES_H

#ifdef BERSIRC_PLUGIN
/* for claro */
#define PLUGIN
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#ifdef ENV_GTK
#include <errno.h>
#endif

#ifdef ENV_WIN32
#include <shlobj.h>

#ifndef SHGFP_TYPE_CURRENT
// MingW doesn't currently define this one
#define SHGFP_TYPE_CURRENT 0
#endif

#endif

#include "version.h"

#include <claro/graphics.h>

/* cheap porting :) */

#define CWidget object_t
#define ClaroIcon image_t
#define ClaroTable void
#define CMenuItem list_item_t
#define CEvent event_t
#define ClaroTableRow void
#define CToolItem list_item_t
#define CFont void
#define ClaroTableCell void

#define c_new_window(p,text,w,h,f) \
	window_widget_create( p, new_bounds(-1,-1,w,h), f )

#define c_new_aligner(p,f) \
	container_widget_create(p,new_bounds(0,0,100,100),f)

#define c_new_text(p,text,x,y,w,h,f) \
	textbox_widget_create( p, new_bounds(x,y,w,h), f )

#define c_new_groupbox(p,text,x,y,w,h,f) \
	frame_widget_create( p, new_bounds(x,y,w,h), text, f )

#define c_new_checkbox(p,text,x,y,w,h,f) \
	checkbox_widget_create( p, new_bounds(x,y,w,h), text, f )

#define c_new_pushbutton(p,text,x,y,w,h,f) \
	button_widget_create_with_label( p, new_bounds(x,y,w,h), f, text )

#define c_new_listbox(p,x,y,w,h,f) \
	listbox_widget_create( p, new_bounds(x,y,w,h), f )

#define c_new_label(p,text,x,y,w,h,f) \
	label_widget_create( p, new_bounds(x,y,w,h), text, f )

/*#define c_new_canvas(p,x,y,w,h,f) \
	canvas_widget_create( p, new_bounds(x,y,w,h), f )*/
#define c_new_canvas(...) 0

#define c_new_workspace(p,x,y,w,h,f) \
	workspace_widget_create( p, new_bounds(x,y,w,h), f )

#define c_new_statusbar(p,text,f) \
	statusbar_widget_create( p, f )

#define c_new_menubar(p,f) \
	menubar_widget_create( p, f )

#define c_new_toolbar(p,f) \
	toolbar_widget_create( p, f )

#define c_init(t,f) \
	claro_base_init( ); \
	claro_graphics_init( ); \
	log_fd_set_level( CL_DEBUG, stderr ); \
	clog( CL_INFO, "%s (%s) running using Claro!", t, __FILE__ );

#define c_widget_focus(w) \
	widget_focus( WIDGET(w) )

#define c_menubar_add_item_text(w,p,n) \
	menubar_append_item(w,p,0,n)

#define c_menubar_add_item_text_icon(w,p,n,i) \
	menubar_append_item(w,p,0,n)

#define c_menubar_add_separator(w,p) \
	menubar_append_separator(w,p)

#define c_menubar_attach(i,h) \
	object_addhandler(OBJECT(i),"pushed",h)

#define c_toolbar_attach(i,h) \
	object_addhandler(OBJECT(i),"pushed",h)

#define c_toolbar_add_item(p,i,n) \
	toolbar_append_icon(p,i,0,0)

#define c_cleanup()

#define c_checkbox_checked(c,v) checkbox_set_checked(c,v)

/*#define c_new_event_handler(o,e,h) \
	object_addhandler(o,e,h)*/
#define c_new_event_handler(...) 0

#define c_text_settext(w,t) \
	textbox_set_text(w,t)

#define c_window_settitle(w,t) \
	window_set_title(w,t)

#define c_workspace_get_active(w) \
	workspace_get_active(w)

#define c_widget_hide(w) \
	widget_hide(WIDGET(w))

#define c_widget_show(w) \
	widget_show(WIDGET(w))

#define c_workspace_tile(w,t) \
	workspace_tile(w,t)

#define c_workspace_cascade(w) \
	workspace_cascade(w)

#define c_mainloop claro_loop

#define C_MENU_POPUP 0

#define C_EVENT_MINIMIZED "minimized"
#define C_EVENT_MAXIMIZED "maximized"
#define C_EVENT_RESTORED "restored"
#define C_EVENT_RESIZE "resized"
#define C_EVENT_MOVE "moved"
#define C_EVENT_MOUSE_LEFT_CLICK "clicked"
#define C_EVENT_MOUSE_LEFT_RELEASE "released"
#define C_EVENT_MOUSE_LEFT_DBLCLK "double_clicked"
#define C_EVENT_MOUSE_RIGHT_CLICK "right_clicked"
#define C_EVENT_MOUSE_RIGHT_RELEASE "right_released"
#define C_EVENT_MOUSE_WHEEL "scroll_wheel"
#define C_EVENT_MOUSE_MOVE "mouse_moved"
#define C_EVENT_DRAW "redraw"
#define C_EVENT_DESTROY "destroy"
#define C_EVENT_PREDRAW "PORTHACK_predraw"
#define C_EVENT_ONFOCUS "focus"
#define C_EVENT_TEXT_ENTER "PORTHACK_enter"
#define C_EVENT_KEY_PRESS "PORTHACK_keydown"
#define C_EVENT_DIALOG_OK "PORTHACK_dialog_ok"
#define C_EVENT_BUTTON_PRESS "PORTHACK_button_press"

#define C_WORKSPACE_TILE_HORIZ cWorkspaceTileHorizontally
#define C_WORKSPACE_TILE_VERT cWorkspaceTileVertically

// FIXME: these are in claro but i CBF right now
#define c_add_mainloop(...)

// FIXME: new claro is missing these
#define c_window_minsize(...)
#define c_window_maxsize(...)
#define c_window_set_icon(...)
#define c_destroy_widget(...) // NOT just widget_destroy !! :)
#define c_icon_from_xpm(a) 0
#define c_new_systrayicon(...) 0
#define c_menubar_popup(...)
#define c_menubar_disable_item(...)
#define c_set_scroll(...)
#define c_set_scroll_pos(...)
#define c_get_scroll_pos(...) 0
#define c_canvas_invalid(...)
#define c_send_event_singular(...)
#define c_font_clone(...) 0
#define c_font_bind_widget(...)
#define c_close_widget(...)
#define c_scroll_page(...)
#define c_text_setsel(...)
#define c_window_flash(...)
#define c_tbl_empty(...)
#define c_canvas_paint_clear(...)
#define c_font_create(...) 0
#define c_widget_maxsize(...)
#define c_widget_maximize(...)
#define c_widget_restore(...)
//#define c_new_aligner(...) 0
#define c_tbl_create(...) 0
#define c_aligner_set_stretchable_child(...)
#define c_listbox_attach_table(...)
#define c_shutdown(...)
#define c_open_browser(...)
#define c_widget_close(...)
#define c_new_image(...)
#define c_listview_selected(...) 0
#define c_listbox_selected(...) 0
#define c_new_dropdown(...) 0
#define c_widget_disable(...) 0
#define c_widget_enable(...) 0

#define C_MDICHILD 0
#define C_DIALOG 0
#define C_WINDOW_CENTER_TO_PARENT 0

#define C_WANT_KEYPRESS 0

#define C_ALIGNER_HORIZONTAL 0
#define C_ALIGNER_VERTICAL 0

#define C_START_MAXIMISE 0
#define C_BORDER_NONE 0
#define C_SCROLLING_VERT 0
#define C_SCROLLING_GREY_NOT_HIDE 0

#define CK_UP -1
#define CK_DOWN -1
#define CK_PAGEUP -1
#define CK_PAGEDOWN -1
#define CK_ENTER -1

/* cheap porting :) */

#include "core.h"

#include "ircview.h"
#include "net/socket.h"
#include "dcc.h"
#include "xml/xml.h"
#include "irc.h"
#include "commands.h"
#include "modes.h"
#include "messages.h"
#include "lang.h"
#include "profile.h"
#include "netbrowser.h"
#include "connectwin.h"
#include "taskbar.h"

#include "options.h"

#include "configdir.h"

#include "plugins.h"

#include "dccwin.h"

#include "urlwin.h"

#include "joinwin.h"

#include "aboutwin.h"

#include "autowin.h"

#include "windows.h"
#include "connection.h"
#include "server.h"

#endif
