/*************************************************************************
$Id: irc_cmd.h 175 2005-10-17 02:54:21Z terminal $

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

void b_run_command( BServerWindow *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window );

#define BERS_COMMAND(x) int x( BServerWindow *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window )

#ifndef BERSIRC_PLUGIN

/* /server,srv,svr,nserver,nsrv,nsvr
 *
 ! Connects to an IRC server
 ? /server <server> [port]
 : Connects to the IRC server <server> on port [port] or 6667 if not specified. \\
 : The current connection (if any) is closed and replaced with the new one.
 ? /nserver <server> [port]
 ? /server -new <server> [port]
 : Same as above, but a new status window is opened, leaving the old connection(s) \\
 : (if any) running.
 */
BERS_COMMAND( b_cmd_server );

/* /quit
 *
 ! Quits the current IRC server
 ? /quit [reason]
 : Quits from the IRC server active in the window this is entered in. The optional \\
 : [reason] parameter specifies the quit message to send to the server. If no reason \\
 : is specified, the default one (from preferences) is used.
 */
BERS_COMMAND( b_cmd_quit );

/* /join,j
 *
 ! Joins one or more channels
 ? /join #channel1[,#channel2[,#channel3[,#...]]]
 : Joins the specified channel(s) on the active server.
 ? /join #channel [key]
 : Joins the specified channel using the key specified by [key]. This is only applicable \\
 : for channels where keys are set and required.
 */
BERS_COMMAND( b_cmd_join );

/* /part,p
 *
 ! Part one or more channels
 ? /part #channel1[,#channel2[,#channel3[,#...]]]
 : Parts the specified channel(s) on the active server.
 */
BERS_COMMAND( b_cmd_part );

/* /nick
 *
 ! Changes your nickname
 ? /nick <nickname>
 : Attempts to change your nickname to <nickname>.
 */
BERS_COMMAND( b_cmd_nick );

/* /msg
 *
 ! Sends a message to a channel or user
 ? /msg <user/channel> <message>
 : Sends the message specified by <message> to the channel or user specified.
 & query
 */
BERS_COMMAND( b_cmd_privmsg );

/* /action
 *
 ! Sends an action to a channel or user
 ? /action <user/channel> <action>
 : Sends the action specified by <action> to the channel or user specified.
 :
 : Actions appear to users similar to:
 :  * <nick> <action>
 :
 : So, for example if you are called 'Bersie':
 :  /action #lobby is happy
 : Everyone in #lobby will see:
 :  * Bersie is happy
 & me
 */
BERS_COMMAND( b_cmd_action );

/* /query
 *
 ! Opens a query window to a user
 ? /query <user> [message]
 : Opens a query window to the user <user>. The option message parameter allows \\
 : you to specify a message to be sent automatically once open.
 & msg
 */
BERS_COMMAND( b_cmd_query );

/* /me
 *
 ! Sends an action to the current channel or user
 ? /me <action>
 : Sends the action specified by <action> to the active channel or user.
 :
 : Actions appear to users similar to:
 :  * <nick> <action>
 :
 : So, for example if you are called 'Bersie':
 :  /me is happy
 : Everyone in the current window will see:
 :  * Bersie is happy
 & action
 */
BERS_COMMAND( b_cmd_me );

/* /raw,quote
 *
 ! Sends text raw to the IRC server
 ? /raw <text>
 : Sends the message specified by <text> directly to the IRC server without parsing
 */
BERS_COMMAND( b_cmd_raw );

/* /clear
 *
 ! Clears the text in the current window
 ? /clear
 : Clears all text in the currently active window
 */
BERS_COMMAND( b_cmd_clear );

/* /close
 *
 ! Closes the current window
 ? /close
 : Closes the currently active window
 */
BERS_COMMAND( b_cmd_close );

/* CH/kick
 *
 ! Kicks a user from a channel
 ? /kick <user> [reason]
 : Kicks <user> from the current channel with the optional [reason].
 */
BERS_COMMAND( b_cmd_kick );

/* CO/op
 *
 ! Gives channel op to a user
 ? /op [user1] [user2] [...]
 : Gives channel op to [user1], [user2], ...  (or yourself if not specified) \\
 : in the active channel
 */
/* CO/deop
 *
 ! Revokes channel op from a user
 ? /op [user1] [user2] [...]
 : Removes channel op from [user1], [user2], ...  (or yourself if not specified) \\
 : in the active channel
 */
/* CO/halfop,hop
 *
 ! Gives channel half-op to a user
 ? /halfop [user1] [user2] [...]
 : Gives channel half-op to [user1], [user2], ...  (or yourself if not specified) \\
 : in the active channel
 :
 : Note: Half-op is not supported on all IRC servers/networks.
 */
/* CO/dehalfop,dehop
 *
 ! Revokes channel voice from a user
 ? /devoice [user1] [user2] [...]
 : Removes channel half-op from [user1], [user2], ...  (or yourself if not specified) \\
 : in the active channel
 :
 : Note: Half-op is not supported on all IRC servers/networks.
 */
/* CH/voice,vop
 *
 ! Gives channel voice to a user
 ? /voice [user1] [user2] [...]
 : Gives channel voice to [user1], [user2], ...  (or yourself if not specified) \\
 : in the active channel
 */
/* CH/devoice,devop
 *
 ! Revokes channel voice from a user
 ? /devoice [user1] [user2] [...]
 : Removes channel voice from [user1], [user2], ...  (or yourself if not specified) \\
 : in the active channel
 */
BERS_COMMAND( b_cmd_quickmode );

/* /cycle
 *
 ! Cycles (parts/joins) a channel
 ? /cycle [channel]
 : Cycles (parts then rejoins) [channel] (or active channel if not specified)
 */
BERS_COMMAND( b_cmd_cycle );

/* /topic
 *
 ! Sets or retreives the topic for a channel
 ? /topic [channel]
 : Retreives the topic for the [channel] (or active channel if not specified)
 ? /topic <channel> <topic>
 : Sets the topic for <channel> to <topic>
 */
BERS_COMMAND( b_cmd_topic );

/* /whois
 *
 ! Retreives information about a user on IRC
 ? /whois <user>
 : Retreives information about <user> on active IRC server
 */
/* /whowas
 *
 ! Retreives information about a user on IRC
 ? /whowas <user>
 : Retreives previous login information about <user> on the active IRC server
 */
BERS_COMMAND( b_cmd_default );

/* /ctcp
 *
 ! Sends a CTCP command to user
 ? /ctcp <user> <command>
 : Sends CTCP <command> to <user>
 */
BERS_COMMAND( b_cmd_ctcp );

/* /dcc
 *
 ! Send and Receive files via DCC
 ? /dcc <option> [<user> <filename>]
 : send or accept DCC request from <user>
 */
BERS_COMMAND( b_cmd_dcc );

// ex-menu commands
BERS_COMMAND( b_cmd_exit );
BERS_COMMAND( b_cmd_newstatus );
BERS_COMMAND( b_cmd_profile );
BERS_COMMAND( b_cmd_netbrowse );
BERS_COMMAND( b_cmd_go );
BERS_COMMAND( b_cmd_connectwin );
BERS_COMMAND( b_cmd_tile );
BERS_COMMAND( b_cmd_casc );
BERS_COMMAND( b_cmd_options );
BERS_COMMAND( b_cmd_chanstats );
BERS_COMMAND( b_cmd_comefg );
BERS_COMMAND( b_cmd_dccwin );
BERS_COMMAND( b_cmd_savetext );
BERS_COMMAND( b_cmd_urlwin );
BERS_COMMAND( b_cmd_about );
BERS_COMMAND( b_cmd_joinwin );
BERS_COMMAND( b_cmd_autowin );

BERS_COMMAND( b_cmd_dns );

BERS_COMMAND( b_msg_operwall );

#endif

//
