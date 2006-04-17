/*************************************************************************
$Id: irc.h 186 2005-11-03 23:35:26Z terminal $

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

//

#define BERSTAG " [ http://www.bersirc.org/ - Open Source IRC ]"

//

#define B_CMD_WINDOW_STATUS 1
#define B_CMD_WINDOW_CHANNEL 2
#define B_CMD_WINDOW_QUERY 4
#define B_CMD_WINDOW_CHAT 6
#define B_CMD_WINDOW_ALL 7
#define B_CMD_WINDOW_NONE 8

#define B_CMD_CLIENTONLY 32

//

extern char *quitMessages[];

typedef struct busr
{
	char nickname[64];
	char username[64];
	char hostname[128];
	char realname[128];
	
	char modes[16];
	
	ClaroTableRow *row;
	
	struct busr *next;
	struct busr *prev;
} BUserStore;

typedef struct
{
	char nickname[256];
	char username[256];
	char hostname[256];
} BUserMask;

typedef struct b_trc
{
	char *input;
	int reserved;
	
	struct b_trc *next;
	struct b_trc *prev;
} BTextRecall;

typedef struct b_win_contextm
{
	CWidget *menu;
	
	CMenuItem *close_window;
	CMenuItem *clear_buffer;
	
	CMenuItem *reserved[7];
} BWindowContextMenu;

#define B_WINDOW_MAXIMISE 1
#define B_WINDOW_MINIMISE 2

#define B_WINDOW_SHARED_STRUCT \
	int type; \
	\
	BTextRecall *recall_newest;\
	BTextRecall *recall_shown;\
	int recall_mode;\
	\
	BWindowContextMenu conmenu;\
	\
	int taskbar_flags;\
	\
	object_t *window;\
	object_t *input;\
	layout_t *layout;\
	object_t *content;\
	object_t *container;\
	layout_t *ct_layout;\
	object_t *splitter;\
	object_t *scroll;\
	\
	int window_flags;\
	

//

typedef struct b_cw
{
	B_WINDOW_SHARED_STRUCT
	
	// before this point, Chat and Server window structures are identical.
	
	CWidget *userlist;
	
	char dest[256];
	char topic[512];
	char key[256];
	
	ClaroTable *channel_modes;
	
	BUserStore *users; // just for channels (user list)
	ClaroTable *users_table;
	int synced;
	
	int parted;
	int partrequested;
	
	void *server;
} BChatWindow;

typedef struct b_sw
{
	B_WINDOW_SHARED_STRUCT

	// before this point, Chat and Server window structures are identical.
	
	char server_network[128];
	char server_name[128];
	int server_port;
	
	CSocket *sock;
	char title[256];
	
	char servername[256];
	int connected;
	
	int welcomed;
	
	char user_hostname[128];
	
	char *onconnect;
	int onconnect_len;
	
	char nickname[256];
	char umode[32];
	int nickcount;
	char password[64];
	
	list_t chat_windows;
} BServerWindow;

typedef struct b_cmd
{
	char command[256];
	
	int (*function)( BServerWindow *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window );
	
	int flags;
	
	struct b_cmd *next;
	struct b_cmd *prev;
} BUserCommand;

typedef struct
{
	char face[128];
	int size;
} BFontDef;

//

extern XMLItem *xidentity;
extern CWidget *workspace;

//

// from berscore
image_t *b_icon( char *name );

//

void b_userstore_updated( BChatWindow *win, BUserStore *user, int relocate );

void b_window_focus( CWidget *w );
BServerWindow *b_find_any_by_widget( CWidget *w );

char *b_get_quit_message( );

BServerWindow *b_server_find_active( );

BServerWindow *b_new_server_window( );
BChatWindow *b_new_chat_window( BServerWindow *server, char *dest, int flags );
BUserStore *b_chat_user_find_nick( BChatWindow *win, char *nick );

void b_menu_from_xml( XMLFile *xfile, CWidget *menu, char *name, int type );

void b_identity_getautonick( int num, char *buf );

void b_server_update_title( BServerWindow *server );
void b_chat_update_title( BChatWindow *cw );

void b_chat_user_add( BChatWindow *win, BUserStore *user );
void b_chat_user_del( BChatWindow *win, char *nick );
BUserStore *b_chat_user_find( BChatWindow *win, char *nick );
void b_chat_user_empty( BChatWindow *win );

int b_chatwin_printf( BChatWindow *cwin, int colour, char *fmt, ... );

BChatWindow *b_find_chat_by_dest( BServerWindow *win, char *dest );
BChatWindow *b_find_chat_by_widget( CWidget *w );
BServerWindow *b_find_server_by_widget( CWidget *w );
BServerWindow *b_find_server_by_sname( char *data, int connected );
BServerWindow *b_find_server_by_conn( int connected );

void b_servers_run( );
int b_user_command( void *inwin, char *cmdi, int fromtype );

void b_init_commands( );

int b_server_printf( BServerWindow *server, char *fmt, ... );
int b_swindow_printf( BServerWindow *server, int colour, char *fmt, ... );

void b_server_run_onconnect( BServerWindow *sw );
void b_server_onconnect_goto( BServerWindow *sw, char *place );

void b_register_message( char *command, void *function, int flags );
void b_register_command( char *command, void *function, int flags );


/** new */

void b_widget_set_font( object_t *w, char *loc );
void b_identity_attrib( char *attrib, char *buf );

//
