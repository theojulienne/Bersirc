/*************************************************************************
$Id: irc_msg.h 186 2005-11-03 23:35:26Z terminal $

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

void b_run_message( BServerWindow *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window );
char b_find_user_prefix( BUserStore *st );
void b_server_message( BServerWindow *win, char *cmdi );
void b_init_messages( );



#define BERS_MESSAGE(x) int x( BServerWindow *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window )

#define B_MODE_HANDLER_PARMS BServerWindow *server, BChatWindow *chanwin, char dir, char mode, char *channel, char *victim

#define BERS_MESSAGE_FROM() (BUserMask *)window

#ifndef BERSIRC_PLUGIN
BERS_MESSAGE( b_msg_ping );
BERS_MESSAGE( b_msg_privmsg );
BERS_MESSAGE( b_msg_notice );
BERS_MESSAGE( b_msg_welcome );
BERS_MESSAGE( b_msg_uphost );
BERS_MESSAGE( b_msg_motd );
BERS_MESSAGE( b_msg_mode );
BERS_MESSAGE( b_msg_join );
BERS_MESSAGE( b_msg_part );
BERS_MESSAGE( b_msg_quit );
BERS_MESSAGE( b_msg_kick );
BERS_MESSAGE( b_msg_topic );
BERS_MESSAGE( b_msg_topicset );
BERS_MESSAGE( b_msg_channames );
BERS_MESSAGE( b_msg_channames_end );
BERS_MESSAGE( b_msg_nickused );
BERS_MESSAGE( b_msg_nick );
BERS_MESSAGE( b_msg_unknowncmd );
BERS_MESSAGE( b_msg_writeparm1 );
BERS_MESSAGE( b_msg_writeparm2 );
BERS_MESSAGE( b_msg_away );
BERS_MESSAGE( b_msg_whois_311 );
BERS_MESSAGE( b_msg_whois_is );
BERS_MESSAGE( b_msg_whois_319 );
BERS_MESSAGE( b_msg_whois_312 );
BERS_MESSAGE( b_msg_whois_317 );
BERS_MESSAGE( b_msg_whois_338 );
BERS_MESSAGE( b_msg_whois_314 );
BERS_MESSAGE( b_msg_whois_330 );
BERS_MESSAGE( b_msg_invite_341 );
BERS_MESSAGE( b_msg_topic_change );
BERS_MESSAGE( b_msg_error );
BERS_MESSAGE( b_msg_connected );
BERS_MESSAGE( b_msg_userhost );
BERS_MESSAGE( b_msg_writeparm2toactive );
BERS_MESSAGE( b_msg_uaway );
BERS_MESSAGE( b_msg_time );
BERS_MESSAGE( b_msg_null );
BERS_MESSAGE( b_msg_checkprefix );
BERS_MESSAGE( b_msg_createtime );
BERS_MESSAGE( b_msg_invite );
BERS_MESSAGE( b_msg_wallops );
BERS_MESSAGE( b_msg_notopic );
BERS_MESSAGE( b_msg_stderror );
BERS_MESSAGE( b_msg_chanurl );
BERS_MESSAGE( b_msg_chanmodeis );
#endif

//
