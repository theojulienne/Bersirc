/*************************************************************************
$Id: irc_msg.c 189 2005-11-08 05:57:12Z terminal $

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

/* UPGRADE FIXME: move to list/node_t */
BUserCommand *scmd_head = 0;

void b_server_message( BServerWindow *win, char *cmdi )
{
	char *cmd, *cmdo, *cmdb;
	char *tok, *tok2;
	char command[256];
	char **params = 0;
	char **paramsbak = 0;
	int a, b, c, d;
	BUserMask *sender;
	
	cmdo = cmd = (char *)malloc( strlen( cmdi ) + 1 );
	strcpy( cmd, cmdi );
	cmdb = (char *)malloc( strlen( cmdi ) + 1 );
	strcpy( cmdb, cmdi );
	
	tok = strtok( cmd, " " );
	
	c = 0;
	sender = 0;
	
	//printf( "%s\n", cmdb );
	
	if ( tok != NULL )
	{
		if ( tok[0] == ':' )
		{
			tok2 = strtok( NULL, " " );
			
			if ( tok2 != NULL )
			{
				c = 1;
				
				sender = (BUserMask *)malloc( sizeof( BUserMask ) );
				memset( sender, 0, sizeof( BUserMask ) );
				
				sscanf( tok + 1, "%[^!]!%[^@]@%s", sender->nickname, sender->username, sender->hostname );
				
				tok = tok2;
			}
		}
		
		// command
		strncpy( command, tok, 256 );
		
		a = 0;
		d = 0;
		
		// parse params
		while ( ( tok = strtok( NULL, " " ) ) != NULL )
		{
			if ( d == 0 )
			{
				if ( tok[0] == ':' )
				{
					d = 1;
					tok++;
				}
				
				// increment number of pointers
				a++;
				
				params = (char **)realloc( params, sizeof( char * ) * a );
				
				if ( d == 0 ) {
					params[a-1] = (char *)malloc( strlen( tok ) + 1 );
					strcpy( params[a-1], tok );
				} else {
					params[a-1] = (char *)malloc( strlen( cmdi + (tok-cmd) ) + 1 );
					strcpy( params[a-1], cmdi + (tok-cmd) );
					
					break; //end
				}
			}
		}
		
		paramsbak = (char **)malloc( sizeof( char * ) * a );
		memcpy( paramsbak, params, sizeof( char * ) * a );
		
		// run it
		b_run_message( win, cmdb, command, params, a, c << 12, sender );
		
		// clear memory
		if ( sender != 0 )
			free( sender );
		
		for ( b = 0; b < a; b++ )
		{
			free( paramsbak[b] );
		}
		
		if ( params != 0 )
			free( params );
		if ( paramsbak != 0 )
			free( paramsbak );
	}
	
	free( cmdo );
	free( cmdb );
}

void b_register_message( char *command, void *function, int flags )
{
	BUserCommand *cmd;
	
	cmd = (BUserCommand *)malloc( sizeof( BUserCommand ) );
	
	memset( cmd, 0, sizeof( BUserCommand ) );
	
	cmd->prev = 0;
	cmd->next = scmd_head;
	
	if ( scmd_head != 0 )
		scmd_head->prev = cmd;
	
	scmd_head = cmd;
	
	strncpy( cmd->command, command, 256 );
	cmd->function = function;
	cmd->flags = flags;
}

void b_init_messages( )
{
	/*** Messages ***/
	
	b_register_message( "ping", &b_msg_ping, 0 );
	b_register_message( "privmsg", &b_msg_privmsg, 0 );
	b_register_message( "notice", &b_msg_notice, 0 );
	b_register_message( "mode", &b_msg_mode, 0 );
	b_register_message( "join", &b_msg_join, 0 );
	b_register_message( "part", &b_msg_part, 0 );
	b_register_message( "quit", &b_msg_quit, 0 );
	b_register_message( "kick", &b_msg_kick, 0 );
	b_register_message( "nick", &b_msg_nick, 0 );
	b_register_message( "topic", &b_msg_topic_change, 0 );
	b_register_message( "error", &b_msg_error, 0 );
	
	b_register_message( "invite", &b_msg_invite, 0 );
	b_register_message( "wallops", &b_msg_wallops, 0 );
	
	// numerics
	b_register_message( "001", &b_msg_welcome, 0 );
	b_register_message( "001", &b_msg_connected, 0 );
	b_register_message( "002", &b_msg_welcome, 0 );
	b_register_message( "003", &b_msg_welcome, 0 );
	b_register_message( "004", &b_msg_welcome, 0 );
	b_register_message( "004", &b_msg_uphost, 0 );
	b_register_message( "005", &b_msg_checkprefix, 0 );
	b_register_message( "005", &b_msg_checkchanmodes, 0 );
	b_register_message( "005", &b_msg_welcome, 0 );
	b_register_message( "006", &b_msg_welcome, 0 );
	b_register_message( "007", &b_msg_welcome, 0 );
	b_register_message( "008", &b_msg_welcome, 0 );
	
	// end of *
	b_register_message( "015", &b_msg_writeparm1, 0 );
	b_register_message( "017", &b_msg_writeparm1, 0 );
			
	// 200: RPL_TRACELINK "Link <version & debug level> <destination> <next server>"
	// 201: RPL_TRACECONNECTING "Try. <class> <server>"
	// 202: RPL_TRACEHANDSHAKE "H.S. <class> <server>"
	// 203: RPL_TRACEUNKNOWN "???? <class> [<client IP address in dot form>]"
	// 204: RPL_TRACEOPERATOR "Oper <class> <nick>"
	// 205: RPL_TRACEUSER "User <class> <nick>"
	// 206: RPL_TRACESERVER "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>"
	// -
	// 208: RPL_TRACENEWTYPE "<newtype> 0 <client name>"
	// 209: RPL_TRACECLASS - RESERVED
	// -
	// 210: ??
	b_register_message( "210", &b_msg_writeparm1, 0 );
	// 211: RPL_STATSLINKINFO "<linkname> <sendq> <sent messages> <sent bytes> 
	//                         <received messages> <received bytes> <time open>"
	// 212: RPL_STATSCOMMANDS "<command> <count>"
	// 213: RPL_STATSCLINE "C <host> * <name> <port> <class>"
	// 214: RPL_STATSNLINE "N <host> * <name> <port> <class>"
	// 215: RPL_STATSILINE "I <host> * <host> <port> <class>"
	// 216: RPL_STATSKLINE "K <host> * <username> <port> <class>"
	// 217: RPL_STATSQLINE - RESERVED
	// -
	// 218: RPL_STATSYLINE "Y <class> <ping frequency> <connect frequency> <max sendq>"
	// 219: RPL_ENDOFSTATS "<stats letter> :End of /STATS report"
	b_register_message( "219", &b_msg_writeparm2, 0 );
	// -
	// 221: RPL_UMODEIS "<user mode string>"
	// -
	// 231: RPL_SERVICEINFO - RESERVED
	// 232: RPL_ENDOFSERVICES - RESERVED
	// 233: RPL_SERVICE - RESERVED
	// 234: RPL_SERVLIST - RESERVED
	// 235: RPL_SERVLISTEND - RESERVED
	// -
	// 241: RPL_STATSLLINE "L <hostmask> * <servername> <maxdepth>"
	// 242: RPL_STATSUPTIME ":Server Up %d days %d:%02d:%02d"
	b_register_message( "242", &b_msg_writeparm1, 0 );
	// 243: RPL_STATSOLINE "O <hostmask> * <name>"
	// 244: RPL_STATSHLINE "H <hostmask> * <servername>"
	// -
	// 250: ??
	b_register_message( "250", &b_msg_writeparm1, 0 );
	// 251: RPL_LUSERCLIENT ":There are <integer> users and <integer> invisible on <integer> servers"
	b_register_message( "251", &b_msg_welcome, 0 );
	// 252: RPL_LUSEROP "<integer> :operator(s) online"
	b_register_message( "252", &b_msg_welcome, 0 );
	// 253: RPL_LUSERUNKNOWN "<integer> :unknown connection(s)"
	b_register_message( "253", &b_msg_welcome, 0 );
	// 254: RPL_LUSERCHANNELS "<integer> :channels formed"
	b_register_message( "254", &b_msg_welcome, 0 );
	// 255: RPL_LUSERME ":I have <integer> clients and <integer> servers"
	b_register_message( "255", &b_msg_welcome, 0 );
	// 256: RPL_ADMINME "<server> :Administrative info"
	// 257: RPL_ADMINLOC1 ":<admin info>"
	// 258: RPL_ADMINLOC2 ":<admin info>"
	// 259: RPL_ADMINEMAIL ":<admin info>"
	// -
	// 261: RPL_TRACELOG "File <logfile> <debug level>"
	// -
	// ??
	b_register_message( "265", &b_msg_welcome, 0 );
	// ??
	b_register_message( "266", &b_msg_welcome, 0 );
	// -
	// 290: ??
	b_register_message( "290", &b_msg_writeparm1, 0 );
	// -
	// 292: ??
	b_register_message( "292", &b_msg_writeparm1, 0 );
	// -
	// 294: ??
	b_register_message( "294", &b_msg_writeparm1, 0 );
	
	// ---
	
	// 300: Dummy reply number. Not used.
	// 301: RPL_AWAY
	b_register_message( "301", &b_msg_uaway, 0 );
	// 302: RPL_USERHOST
	b_register_message( "302", &b_msg_userhost, 0 );
	// 303: RPL_ISON
	// -
	// 305: You are no longer marked as being away
	b_register_message( "305", &b_msg_away, 0 ); // FIXME: language
	// 306: You have been marked as being away
	b_register_message( "306", &b_msg_away, 0 ); // FIXME: language
	// 307: ??
	b_register_message( "307", &b_msg_whois_is, 0 );
	// -
	// 310: ??
	b_register_message( "310", &b_msg_whois_is, 0 );
	// 311: RPL_WHOISUSER "<nick> <user> <host> * :<real name>"
	b_register_message( "311", &b_msg_whois_311, 0 );
	// 312: RPL_WHOISSERVER "<nick> <server> :<server info>"
	b_register_message( "312", &b_msg_whois_312, 0 );
	// 313: RPL_WHOISOPERATOR "<nick> :is an IRC operator"
	b_register_message( "313", &b_msg_whois_is, 0 );
	// 314: RPL_WHOWASUSER "<nick> <user> <host> * :<real name>"
	b_register_message( "314", &b_msg_whois_314, 0 );
	// -
	// 315: RPL_ENDOFWHO "<name> :End of /WHO list"
	// 316: RPL_WHOISCHANOP - RESERVED
	// 317: RPL_WHOISIDLE "<nick> <integer> :seconds idle"
	b_register_message( "317", &b_msg_whois_317, 0 );
	// 318: RPL_ENDOFWHOIS "<nick> :End of /WHOIS list"
	b_register_message( "318", &b_msg_null, 0 );
	// 319: RPL_WHOISCHANNELS "<nick> :{[@|+]<channel><space>}"
	b_register_message( "319", &b_msg_whois_319, 0 );
	// 320: ??
	b_register_message( "320", &b_msg_whois_is, 0 );
	// -
	// 321: RPL_LISTSTART "Channel :Users  Name"
	// 322: RPL_LIST "<channel> <# visible> :<topic>"
	// 323: RPL_LISTEND ":End of /LIST"
	// 324: RPL_CHANNELMODEIS "<channel> <mode> <mode params>"
	b_register_message( "324", &b_msg_chanmodeis, 0 );
	// -
	// 328: ??
	b_register_message( "328", &b_msg_chanurl, 0 );
	// 329: ??
	b_register_message( "329", &b_msg_createtime, 0 );
	// 330: ??
	b_register_message( "330", &b_msg_whois_330, 0 );
	// 331: RPL_NOTOPIC "<channel> :No topic is set"
	b_register_message( "331", &b_msg_notopic, 0 );
	// 332: RPL_TOPIC "<channel> :<topic>"
	b_register_message( "332", &b_msg_topic, 0 );
	// 333: ??
	b_register_message( "333", &b_msg_topicset, 0 );
	// -
	// 338: ??
	b_register_message( "338", &b_msg_whois_338, 0 );
	// -
	// 341: RPL_INVITING "<channel> <nick>"
	b_register_message( "341", &b_msg_invite_341, 0 );
	// 342: RPL_SUMMONING "<user> :Summoning user to IRC"
	// -
	// 351: RPL_VERSION "<version>.<debuglevel> <server> :<comments>"
	// 352: RPL_WHOREPLY "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
	// 353: RPL_NAMREPLY "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
	b_register_message( "353", &b_msg_channames, 0 );
	// -
	// 361: RPL_KILLDONE - RESERVED
	// 362: RPL_CLOSING - RESERVED
	// 363: RPL_CLOSEEND - RESERVED
	// 364: RPL_LINKS "<mask> <server> :<hopcount> <server info>"
	// 365: RPL_ENDOFLINKS "<mask> :End of /LINKS list"
	// 366: RPL_ENDOFNAMES "<channel> :End of /NAMES list"
	b_register_message( "366", &b_msg_channames_end, 0 );
	// 367: RPL_BANLIST "<channel> <banid>"
	// 368: RPL_ENDOFBANLIST "<channel> :End of channel ban list"
	// 369: RPL_ENDOFWHOWAS "<nick> :End of WHOWAS"
	b_register_message( "369", &b_msg_writeparm2, 0 );
	// 371: RPL_INFO ":<string>"
	// 372: RPL_MOTD ":- <text>"
	b_register_message( "372", &b_msg_motd, 0 );
	// 373: RPL_INFOSTART - RESERVED
	// 374: RPL_ENDOFINFO ":End of /INFO list"
	// 375: RPL_MOTDSTART ":- <server> Message of the day - "
	b_register_message( "375", &b_msg_motd, 0 );
	// 376: RPL_ENDOFMOTD ":End of /MOTD command"
	b_register_message( "376", &b_msg_motd, 0 );
	// 377: glass.webchat.org 377 davidk Z-Default 5606 1137752600 :Last MOTD change information: Fri, 20 Jan 2006 02:23:20 -0800
	// 378: ??
	b_register_message( "378", &b_msg_whois_is, 0 );
	// 379: ??
	b_register_message( "379", &b_msg_whois_is, 0 );
	// -
	// 381: RPL_YOUREOPER ":You are now an IRC operator"
	b_register_message( "381", &b_msg_writeparm1, 0 );
	// 382: RPL_REHASHING "<config file> :Rehashing"
	// -
	// 384: RPL_MYPORTIS - RESERVED
	// -
	// 391: RPL_TIME "<server> :<string showing server's local time>"
	b_register_message( "391", &b_msg_time, 0 );
	// 392: RPL_USERSSTART ":UserID   Terminal  Host"
	// 393: RPL_USERS ":%-8s %-9s %-8s"
	// 394: RPL_ENDOFUSERS ":End of users"
	// 395: RPL_NOUSERS ":Nobody logged in"

	// ---

	// 401: no such nick
	b_register_message( "401", &b_msg_stderror, 0 );
	// 402: no such server
	b_register_message( "402", &b_msg_stderror, 0 );
	// 403: no such channel
	b_register_message( "403", &b_msg_stderror, 0 );
	// 404: cannot send to channel
	b_register_message( "404", &b_msg_stderror, 0 );
	// 405: too many channels
	b_register_message( "405", &b_msg_stderror, 0 );
	// 406: there was no such nickname
	b_register_message( "406", &b_msg_stderror, 0 );
	// 407: too many targets
	b_register_message( "407", &b_msg_stderror, 0 );
	// -
	// 409: No origin specified
	// -
	// 411: No recipient given (<command>)
	// 412: No text to send
	// 413: <mask> :No toplevel domain specified
	// 414: <mask> :Wildcard in toplevel domain
	// -
	// 421: <command> :Unknown command
	b_register_message( "421", &b_msg_unknowncmd, 0 );
	// 422: MOTD File is missing
	// 423: <server> :No administrative info available
	// 424: File error doing <file op> on <file>
	// -
	// 431: No nickname given
	b_register_message( "431", &b_msg_writeparm1, 0 ); // FIXME: language
	// 432: <nick> :Erroneus nickname
	// 433: <nick> :Nickname is already in use
	b_register_message( "433", &b_msg_nickused, 0 );
	// -
	// 436: <nick> :Nickname collision KILL
	// 437: ??
	b_register_message( "437", &b_msg_stderror, 0 );
	// -
	// 441: <nick> <channel> :They aren't on that channel
	// 442: <channel> :You're not on that channel
	b_register_message( "442", &b_msg_stderror, 0 );
	// 443: <user> <channel> :is already on channel
	// 444: <user> :User not logged in
	// 445: SUMMON has been disabled
	// 446: USERS has been disabled
	// -
	// 451: You have not registered
	// -
	// 461: <command> :Not enough parameters
	// 462: You may not reregister
	// 463: Your host isn't among the privileged
	// 464: Password incorrect
	// 465: You are banned from this server
	// 466: ERR_YOUWILLBEBANNED - RESERVED
	// 467: <channel> :Channel key already set
	// -
	// 471: <channel> :Cannot join channel (+l)
	b_register_message( "471", &b_msg_stderror, 0 );
	// 472: <char> :is unknown mode char to me
	// 473: <channel> :Cannot join channel (+i)
	b_register_message( "473", &b_msg_stderror, 0 );
	// 474: <channel> :Cannot join channel (+b)
	b_register_message( "474", &b_msg_stderror, 0 );
	// 475: <channel> :Cannot join channel (+k)
	b_register_message( "475", &b_msg_stderror, 0 );
	// 476: ERR_BADCHANMASK - RESERVED
	// -
	// 481: Permission Denied- You're not an IRC operator
	b_register_message( "481", &b_msg_writeparm1, 0 );
	// 482: <channel> :You're not channel operator
	b_register_message( "482", &b_msg_stderror, 0 );
	// 483: You cant kill a server!
	// -
	// 491: No O-lines for your host
	// 492: ERR_NOSERVICEHOST - RESERVED
	// -
	// 499: ??
	b_register_message( "499", &b_msg_writeparm2toactive, 0 );
	// -
	// 501: Unknown MODE flag
	// 502: Cant change mode for other users
}

void b_run_message( BServerWindow *server, char *origcmd, char *command, char **params, int pcount, int flags, void *window )
{
	BUserCommand *cmd;
	int a = 0, b;
/*	char *c;
	char *ca;*/
	char *tmp;
	
	for ( cmd = scmd_head; cmd != 0; cmd = cmd->next )
	{
		if ( !strcasecmp( command, cmd->command ) )
		{
			if ( ( b = ((*cmd->function)( server, origcmd, command, params, pcount, flags, window )) ) == 1 )
				return;
			
			if ( b == 0 )
				a++;
		}
	}

	if ( a == 0 )
	{
		// We don't understand this message! No commands could make sense of it!
		b = strlen( origcmd ) + strlen( command ) + 64;
		
		tmp = (char *)malloc( b );
		
		sprintf( tmp, "[DEBUG] Unhandled message: %s \002[\002 %s \002]\002", command, origcmd );
		
		c_btv_addline( server->content, tmp, 0, BTV_Error );
		
		free( tmp );
	}
}

// These are the default modes/prefixes, used when a server doesn't tell us
// which modes it supports. Do not edit these. If a server leaves modes out,
// the b_irc_allsym string below should remove the prefix and the user will
// be shown as a normal (no-priv) user.
char b_irc_modes[16] = "ohv";
char b_irc_symbs[16] = "@%+";

// Any of these will be counted as a mode prefix, regardless of whether the
// server uses them as one or not. The IRC RFC specifies the first char of
// a nickname should be a letter, therefore we can assume that no nickname
// will ever start with any of these. If any IRC network uses another mode
// prefix not listed here, slap them around with a mIRC trout then add it.
//
// This is used only for stripping them off, stopping the client doing
// actions on users with their prefix included.
// 
// Any prefixes used on the server that are not in the sent PREFIX
// (or default set, above) will simply be discarded.
char b_irc_allsym[] = "!@#$%^&*()-=_+";

/** Commands returning '1' halt, '0' just continue, '-1'  invalid params **/

BERS_MESSAGE( b_msg_ping )
{
	char *reply = "Bersirc";
	
	if ( pcount >= 1 )
		reply = params[0];
	
	b_server_printf( server, "PONG :%s", reply );
	
	if ( b_get_option_bool( xidentity, "general", "opt_gen_show_ping_pong" ) == 1 )
		b_swindow_printf( server, BTV_PingPong, lang_phrase_quick( "ping_pong" ) );
	
	return 0;
}

BERS_MESSAGE( b_msg_nickused )
{
	if ( server->nickcount == -1 )
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "nick_in_use", "nick", params[1], 0 );
		b_swindow_printf( server, BTV_Message, "%s", lang_tmp_buf );
		return 0;
	}
	
	server->nickcount++;
	b_identity_getautonick( server->nickcount, server->nickname );
	b_server_printf( server, "NICK %s", server->nickname );
	
	return 0;
}

int b_msg_check_ctcp( BServerWindow *server, BChatWindow *chatwin, BUserMask *user, char *msg )
{
	char *cmd, *params;
	char out[1024];
	int s = 0;
	
	if ( msg[0] == '\1' )
	{
		msg++;
		
		if ( msg[strlen(msg)-1] == '\1' )
			msg[strlen(msg)-1] = '\0';
		
		cmd = strtok( msg, " " );
		
		if ( cmd == NULL )
			return 1;
		
		params = strtok( NULL, "" );
		
		memset( out, 0, 1024 );
		if ( params == 0 )
			snprintf( out, 1023, "[%s %s]", user->nickname, cmd );
		else
			snprintf( out, 1023, "[%s %s %s]", user->nickname, cmd, params );
		
		if ( !strcasecmp( cmd, "VERSION" ) )
		{
			if ( params == 0 ) // wants ours
			{
				if ( !b_get_option_bool( xidentity, "general", "opt_misc_hide_version" ) ) // only display if we're not hiding it
					b_server_printf( server, "NOTICE %s :\1VERSION Bersirc v" BERSIRC_VERSION " on " BERSIRC_PLATFORM BERSTAG "\1", user->nickname );
			}
			else
			{
				lang_phrase_parse( out, 1024, "ctcp-reply", "user", user->nickname, "type", cmd, "response", params, 0 );
			}
			// otherwise just display the reply :)
		}
		
		if ( !strcasecmp( cmd, "DCC" ) )
		{
			params = strtok( params, " ");
			if ( !strcasecmp( params, "SEND" ) )
			{
				if ( ( params = strtok( NULL, " " ) ) != 0 )
				{
					CSocket *sock = c_socket_create( );
					sock->data = (struct DCCData*)malloc( sizeof( struct DCCData ) );
					((struct DCCData*)sock->data)->file = (char *)malloc( 256 );
					strcpy( ((struct DCCData*)sock->data)->file, params );
					((struct DCCData*)sock->data)->name = (char *)malloc( 256 );
					strcpy( ((struct DCCData*)sock->data)->name, user->nickname );
					
					if ( ( params = strtok( NULL, " " ) ) != 0 )
					{
						sscanf( params, "%lu", &((struct DCCData*)sock->data)->ip );
						((struct DCCData*)sock->data)->ip = ntohl( ((struct DCCData*)sock->data)->ip );
						
						if ( ( params = strtok( NULL, " " ) ) != 0 )
						{
							sscanf( params, "%lu", &((struct DCCData*)sock->data)->position );
							
							if ( ( params = strtok( NULL, " " ) ) != 0 )
							{
								sscanf( params, "%lu", &((struct DCCData*)sock->data)->fileSize );
								((struct DCCData*)sock->data)->state = 3;
								snprintf( out, 1023, "DCC: %s from %s", ((struct DCCData*)sock->data)->file, user->nickname );
							}
						}
					}
				}
			}
		}
								
		// if an action...
		if ( chatwin == 0 && !strcasecmp( cmd, "ACTION" ) && params != 0 )
		{
			// and we should open new windows on messages
			if ( b_get_option_bool( xidentity, "general", "opt_gen_open_new_query" ) == 1 )
				chatwin = b_new_chat_window( server, user->nickname, 0 ); // open a new one.
		}
		
		if ( chatwin != 0 )
		{
			if ( !strcasecmp( cmd, "ACTION" ) && params != 0 )
			{
				b_chatwin_printf( chatwin, BTV_Action, "* %s %s", user->nickname, params );
				s = 1;
			}
		}
		
		if ( s == 0 )
		{
			bserver_t *sendw = b_active_window( ), *real_serv;
			bchannel_t *cw;
			
			real_serv = sendw;
			
			if ( sendw->type == B_CMD_WINDOW_CHANNEL )
			{
				cw = (bchannel_t *)sendw;
				real_serv = (bserver_t *)cw->server;
			}
			
			if ( real_serv != server )
			{
				// different network! use our status window instead of active.
				sendw = server;
			}
			
			b_window_printf( sendw, BTV_CTCP, "%s", out );
		}
		
		return 1;
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_privmsg )
{
	char *sender = "NULL";
	BUserMask *from = (BUserMask *)window;
	BChatWindow *chatwin;
	
	sender = from->nickname;
	
	if ( pcount < 1 )
		return -1;
	
	chatwin = 0;
	
	// if the destination is ourself, don't try this.
	if ( strcasecmp( params[0], server->nickname ) )
		chatwin = b_find_chat_by_dest( server, params[0] );
	
	if ( chatwin == NULL )
	{
		chatwin = b_find_chat_by_dest( server, sender );
	}

	if ( b_msg_check_ctcp( server, chatwin, from, params[1] ) > 0 )
		return 0;

	if ( chatwin == 0 )
	{
		if ( b_get_option_bool( xidentity, "general", "opt_gen_open_new_query" ) == 1 )
			chatwin = b_new_chat_window( server, sender, 0 );
	}

	if ( chatwin != 0 )
	{
		b_chatwin_printf( chatwin, BTV_Message, "<%s> %s", sender, params[1] );
	}
	else
	{
		b_swindow_printf( server, BTV_Message, "<%s:%s> %s", sender, params[0], params[1] );
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_topic_change )
{
	BChatWindow *chatwin;
	BUserMask *from = (BUserMask *)window;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[0] );
	
	if ( chatwin == NULL )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "topic_change", "nick", from->nickname, "topic", params[1], 0 );
	b_chatwin_printf( chatwin, BTV_Topic, "%s", lang_tmp_buf );
	
	strcpy( chatwin->topic, params[1] );
	b_chat_update_title( chatwin );
	
	return 0;
}

BERS_MESSAGE( b_msg_topic )
{
	BChatWindow *chatwin;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[1] );
	
	if ( chatwin == NULL )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "topic", "topic", params[2], 0 );
	b_chatwin_printf( chatwin, BTV_Topic, "%s", lang_tmp_buf );
	
	strcpy( chatwin->topic, params[2] );
	b_chat_update_title( chatwin );
	
	return 0;
}

BERS_MESSAGE( b_msg_topicset )
{
	BChatWindow *chatwin;
	char currtimes[256];
	time_t curtime;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[1] );
	
	if ( chatwin == NULL )
		return -1;
	
	curtime = atoi( params[3] );
	strftime( currtimes, 256, b_get_option_string( xidentity, "time", "opt_time_long_timestamp" ), localtime(&curtime) );
	
	lang_phrase_parse( lang_tmp_buf, 1024, "topic_set", "nick", params[2], "date", currtimes, 0 );
	b_chatwin_printf( chatwin, BTV_Topic, "%s", lang_tmp_buf );
	
	return 0;
}

int b_update_userlist_sortfunc( const void *a, const void *b )
{
	return strcasecmp( *((char **)a), *((char **)b) );
}

#if 0 /* DEPRECATED */
void b_update_userlist_mode( BChatWindow *chan, char **users, int num, char *prefix )
{
	char tmp[64];
	int a;
	
	qsort( users, num, sizeof(char *), &b_update_userlist_sortfunc );
	
	for ( a = 0; a < num; a++ )
	{
		sprintf( tmp, "%s%s", prefix, users[a] );
		
		/*c_listbox_additem( chan->userlist, tmp );*/
	}
}
#endif

void b_update_userlist( BChatWindow *chan )
{
	
}

char b_mode_p2m( char prefix )
{
	char *m_ptr, *p_ptr, *p_diff;
	
	m_ptr = (char *)&b_irc_modes;
	p_ptr = (char *)&b_irc_symbs;
		
	p_diff = strchr( p_ptr, prefix );
	
	if ( p_diff == NULL )
		return 0;
	
	return *( m_ptr + ( p_diff - p_ptr ) );
}

char b_mode_m2p( char prefix )
{
	char *m_ptr, *p_ptr, *p_diff;
	
	m_ptr = (char *)&b_irc_symbs;
	p_ptr = (char *)&b_irc_modes;
		
	p_diff = strchr( p_ptr, prefix );
	
	if ( p_diff == NULL )
		return 0;
	
	return *( m_ptr + ( p_diff - p_ptr ) );
}

BERS_MESSAGE( b_msg_channames )
{
	BChatWindow *chatwin;
	char *tmp, *cur, *a;
	BUserStore store;
	char mode, umode;
	//char *m_ptr, *p_ptr, *p_diff;
	
	memset( &store, 0, sizeof( BUserStore ) );
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[2] );
	
	if ( chatwin == NULL )
		return -1;
	
	//b_chatwin_printf( chatwin, BTV_Topic, "* Users : %s", params[3] );
	
	if ( chatwin->synced == 1 )
	{
		chatwin->synced = 0;
		b_chat_user_empty( chatwin );
	}
	
	tmp = (char *)malloc( strlen( params[3] ) + 1 );
	
	strcpy( tmp, params[3] );
	
	a = tmp;
	
	while ( ( cur = strtok( a, " " ) ) != NULL )
	{
		mode = cur[0];
		
		if ( strchr( b_irc_allsym, mode ) != NULL )
			cur++;
		else
			mode = 0;
		
		strcpy( store.nickname, cur );
		strcpy( store.modes, "" );
		
		umode = b_mode_p2m( mode );
		
		if ( umode != 0 )
		{
			// Another note here, if we get a "q" or "a" mode, we add "o".
			// Although this may not be true, it's likely to be most of the time.
			
			sprintf( store.modes, "%c%s", umode, ( ( umode == 'q' || umode == 'a' ) ? "o" : "" ) );
		}
		
		b_chat_user_add( chatwin, &store );
		
		a = NULL;
	}
	
	return 0;
}

int b_nickcmp( char *a, char *b )
{
	// a < b   -1 (a,b)
	// a > b   +1 (b,a)
	// a = b   0
	
	char *ap = strchr( b_irc_symbs, a[0] );
	char *bp = strchr( b_irc_symbs, b[0] );
	
	if ( bp == NULL && ap != NULL )
		return -1; // a before b
	
	if ( ap == NULL && bp != NULL )
		return 1; // b before a
	
	if ( ap < bp )
		return -1; // a before b
	
	if ( ap > bp )
		return 1; // b before a
	
	if ( bp != NULL )
		b++;
	
	if ( ap != NULL )
		a++;
	
	return strcasecmp( a, b );
}

char b_find_user_prefix( BUserStore *st )
{
	int a;
	
	for ( a = 0; a < strlen( b_irc_modes ); a++ )
	{
		// highest mode first, break as soon as we hit one.
		if ( strchr( st->modes, b_irc_modes[a] ) )
			return b_irc_symbs[a];
	}
	
	return 0;
}

int b_find_table_row_position( object_t *tbl, BUserStore *st )
{
	int a, b;
	char tuser[64];
	char prefix = b_find_user_prefix( st );
	list_item_t *item;
	
	strcpy( tuser, "" );
	
	if ( prefix != 0 )
		sprintf( tuser, "%c", prefix );
	
	strcat( tuser, st->nickname );
	
	b = listbox_get_rows( tbl );
	for ( a = 0; a < b; a++ )
	{
		item = list_widget_get_row( tbl, 0, a );
		
		if ( b_nickcmp( tuser, (char *)item->data[0] ) == -1 )
			return a;
	}
	
	return b; // should go at the end
}

BERS_MESSAGE( b_msg_channames_end )
{
	BChatWindow *chatwin;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[1] );
	
	if ( chatwin == NULL )
		return -1;
	
	chatwin->synced = 1;
	
	//b_update_userlist( chatwin );
	
	b_user_command( chatwin, "/chanstats", 1 );
	
	return 0;
}

BERS_MESSAGE( b_msg_join )
{
	BChatWindow *cw;
	BUserMask *from = BERS_MESSAGE_FROM();
	BUserStore store;
	
	if ( pcount < 1 )
		return -1;
	
	cw = b_find_chat_by_dest( server, params[0] );
	
	if ( !strcasecmp( from->nickname, server->nickname ) )
	{
		if ( cw == 0 )
		{
			cw = b_new_chat_window( server, params[0], 1 );
			lang_phrase_parse( lang_tmp_buf, 1024, "nowtalking", "chan", params[0], 0 );
		}
		else
		{
			lang_phrase_parse( lang_tmp_buf, 1024, "rejoined", "chan", params[0], 0 );
		}
		c_btv_printf( cw->content, 0, BTV_WindowOpen, "%s", lang_tmp_buf );
		
		// clean up the channel mode list, we're new :)
		b_channel_mode_list_clean( cw );
		
		// mark that channel is active
		cw->parted = 0;
		
		// request the channel's mode list
		b_server_printf( server, "MODE %s", params[0] );
	}
	else
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "fmt_join", "nick", from->nickname, "user", from->username, "host", from->hostname, "chan", params[0], 0 );
		c_btv_printf( cw->content, 0, BTV_UserJoin, "%s", lang_tmp_buf );
		strcpy( store.nickname, from->nickname );
		strcpy( store.username, from->username );
		strcpy( store.hostname, from->hostname );
		strcpy( store.realname, "" );
		strcpy( store.modes, "" );
		
		b_chat_user_add( cw, &store );
		//b_insert_userlist_user( cw, &store );
		//b_update_userlist( cw );
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_part )
{
	BChatWindow *cw;
	BUserMask *from = BERS_MESSAGE_FROM();
	
	if ( pcount < 1 )
		return -1;
	
	cw = b_find_chat_by_dest( server, params[0] );
	
	if ( cw == 0 )
		return 0; // nothing to see here..
	
	if ( !strcasecmp( from->nickname, server->nickname ) )
	{
		cw->parted = 1;
		if ( cw->partrequested == 1 && b_get_option_bool( xidentity, "general", "opt_gen_close_on_part" ) )
		{
			widget_close( cw->window );
		}
		else
		{
			lang_phrase_parse( lang_tmp_buf, 1024, "fmt_self_part", "chan", params[0], 0 );
			c_btv_printf( cw->content, 0, BTV_UserPart, "%s", lang_tmp_buf );
			
			b_chat_user_empty( cw );
			/*b_update_userlist( cw );*/
			
		}
	}
	else
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "fmt_part", "nick", from->nickname, "user", from->username, "host", from->hostname, "chan", params[0], "reason", (pcount == 1 ? "No reason" : params[1]), 0 );
		c_btv_printf( cw->content, 0, BTV_UserPart, "%s", lang_tmp_buf );
		b_chat_user_del( cw, from->nickname );
		
		//b_update_userlist( cw );
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_kick )
{
	BChatWindow *cw;
	BUserMask *from = BERS_MESSAGE_FROM();
	
	if ( pcount < 3 )
		return -1;

	cw = b_find_chat_by_dest( server, params[0] );
	
	if ( cw == 0 )
		return 0; // nothing to see here..
	
	if ( !strcasecmp( params[1], server->nickname ) )
	{
		cw->parted = 1;
		lang_phrase_parse( lang_tmp_buf, 1024, "youkicked", "nick", from->nickname, "chan", params[0], "reason", params[2], 0 );
		c_btv_printf( cw->content, 0, BTV_UserKick, "%s", lang_tmp_buf );
		
		b_chat_user_empty( cw );
		b_update_userlist( cw );
		if ( b_get_option_bool( xidentity, "general", "opt_gen_rejoin_on_kick" ) ) // rejoin on kick
		{
			if ( !strcmp( cw->key, "" ) )
				b_server_printf( server, "JOIN %s", cw->dest );
			else
				b_server_printf( server, "JOIN %s %s", cw->dest, cw->key );
		}
	}
	else
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "userkicked", "nick", from->nickname, "victim", params[1], "chan", params[0], "reason", params[2], 0 );
		c_btv_printf( cw->content, 0, BTV_UserKick, "%s", lang_tmp_buf );
		b_chat_user_del( cw, params[1] );
		
		//b_update_userlist( cw );
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_quit )
{
	BChatWindow *cw;
	BUserMask *from = BERS_MESSAGE_FROM();
	char quitmsg[1024];
	node_t *n;
	
	if ( pcount < 1 )
		return -1;
	
	lang_phrase_parse( quitmsg, 1024, "fmt_quit", "nick", from->nickname, "user", from->username, "host", from->hostname, "reason", params[0], 0 );
	//sprintf( quitmsg, "* %s (%s@%s) has quit IRC (%s)", from->nickname, from->username, from->hostname, params[0] );
	
	LIST_FOREACH( n, server->chat_windows.head )
	{
		cw = (BChatWindow *)n->data;
		
		if ( cw->type == B_CMD_WINDOW_CHANNEL )
		{
			// channel window
			if ( b_chat_user_find_nick( cw, from->nickname ) != 0 )
			{
				c_btv_printf( cw->content, 0, BTV_UserQuit, "%s", quitmsg );
				b_chat_user_del( cw, from->nickname );
				//b_update_userlist( cw );
			}
		}
		else
		{
			// query window
			if ( !strcasecmp( cw->dest, from->nickname ) )
			{
				// query to this user
				
				if ( b_get_option_bool( xidentity, "general", "opt_misc_quit_in_query" ) )
				{
					// write it here too
					c_btv_printf( cw->content, 0, BTV_UserQuit, "%s", quitmsg );
				}
			}
		}
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_error )
{
	b_swindow_printf( server, BTV_Error, "%s", params[0] );
	
	return 0;
}

BERS_MESSAGE( b_msg_notice )
{
	char *sender = "NULL";
	BUserMask *from = (BUserMask *)window;
	BChatWindow *chatwin;
	
	if ( flags & 4096 )
		sender = from->nickname;
	
	if ( pcount < 2 )
		return -1;
		
	chatwin = b_find_chat_by_dest( server, params[0] );
	
	if ( flags & 4096 && chatwin == NULL )
	{
		chatwin = b_find_chat_by_dest( server, sender );
	}
	
	if ( b_msg_check_ctcp( server, chatwin, from, params[1] ) > 0 )
		return 0;
	
	b_swindow_printf( server, BTV_Notice, "-%s- %s", sender, params[1] );
	
	return 0;
}

BERS_MESSAGE( b_msg_welcome )
{
	char *sender = "NULL";
	BUserMask *from = (BUserMask *)window;
	char tmp[1024];
	int a;
	
	server->nickcount = -1;
	
	if ( flags & 4096 )
		sender = from->nickname;
	
	if ( pcount < 2 )
		return -1;
	
	strcpy( server->nickname, params[0] );
	
	/*
	if ( pcount == 2 )
		b_swindow_printf( server, BTV_Message, "%s", params[1] );
	else
		b_swindow_printf( server, BTV_Message, "%s %s", params[1], params[2] );
	*/
	
	strcpy( tmp, "" );
	
	for ( a = 1; a < pcount; a++ )
	{
		if ( a > 1 )
			strcat( tmp, " " );
		strncat( tmp, params[a], 1024 - (strlen(tmp) + 2) );
	}
	
	b_swindow_printf( server, BTV_Message, "%s", tmp );
	
	return 0;
}

BERS_MESSAGE( b_msg_uphost )
{
	if ( pcount < 2 )
		return -1;
	
	memset( server->servername, 0, 256 );
	strncpy( server->servername, params[1], 255 );
	
	b_server_update_title( server );
	
	return 0;
}

BERS_MESSAGE( b_msg_connected )
{
	if ( pcount < 2 )
		return -1;
	
	b_server_run_onconnect( server );
	
	return 0;
}

BERS_MESSAGE( b_msg_userhost )
{
	char *txt;
	
	if ( pcount < 2 )
		return -1;
	
	b_swindow_printf( server, BTV_Message, "%s %s", params[0], params[1] );
	
	if ( !strcasecmp( params[0], server->nickname ) )
	{
		if ( ( txt = strrchr( params[1], '@' ) ) != NULL )
		{
			txt++;
			strcpy( server->user_hostname, txt );
			
			while ( 1 )
			{
				char c = server->user_hostname[strlen(server->user_hostname)-1];
				
				if ( c == '\r' || c == '\n' || c == ' ' || c == '\t' )
					server->user_hostname[strlen(server->user_hostname)-1] = '\0';
				else
					break;
			}
			
			//b_swindow_printf( server, BTV_ServerWelcome, "* You are connecting from %s", server->user_hostname );
		}
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_motd )
{
	char *sender = "NULL";
	BUserMask *from = (BUserMask *)window;
	
	if ( flags & 4096 )
		sender = from->nickname;
	
	if ( pcount < 2 )
		return -1;
	
	b_swindow_printf( server, BTV_MOTD, "%s", params[1] );
	
	return 0;
}

/*
typedef struct
{
	char mode;
	int (*handler)( B_MODE_HANDLER_PARMS );
	short pnum;
	short reserved;
	short chmode;
} ChannelModeHandler;

ChannelModeHandler b_chan_mode_handlers[] = {
	// power, my friend
	{ 'q', b_mode_handle_power, 1, 0 },
	{ 'a', b_mode_handle_power, 1, 0 },
	{ 'o', b_mode_handle_power, 1, 0 },
	{ 'h', b_mode_handle_power, 1, 0 },
	{ 'v', b_mode_handle_power, 1, 0 },
	
	// the rest
	{ 'b', b_mode_handle_null, 1, 0 },
	{ 'e', b_mode_handle_null, 1, 0 },
	{ 'L', b_mode_handle_null, 1, 1 },
	{ 'l', b_mode_handle_null, 1, 1 },
	{ 'f', b_mode_handle_null, 1, 1 },
	{ 'k', b_mode_handle_key, 1, 1 },
	
	// and we need to define any singles we want remembered
	{ 'n', b_mode_handle_null, 0, 1 },
	{ 't', b_mode_handle_null, 0, 1 },
	{ 's', b_mode_handle_null, 0, 1 },
	{ 'p', b_mode_handle_null, 0, 1 },
	{ 'i', b_mode_handle_null, 0, 1 },
	{ 'm', b_mode_handle_null, 0, 1 },
	
	// that's it!
	{ 0 }
};
*/

/*
This is a list of channel modes according to 4 types.
A = Mode that adds or removes a nick or address to a list. Always has a parameter.
B = Mode that changes a setting and always has a parameter.
C = Mode that changes a setting and only has a parameter when set.
D = Mode that changes a setting and never has a parameter.

Note: Modes of type A return the list when there is no parameter present.

Note: Some clients assumes that any mode not listed is of type D.

Note: Modes in PREFIX are not listed but could be considered type B. 

[source: http://www.irc.org/tech_docs/005.html ]

CHANMODES=eIbq,k,flj,imnpstrcgzLPQF
*/

BERS_MESSAGE( b_msg_checkchanmodes )
{
	char *cmodes;
	char *tmp, *loc;
	int type, mflags;
	
	cmodes = strstr( origcmd, "CHANMODES=" );
	
	if ( cmodes == 0 )
		return 0;
	
	tmp = strdup( cmodes );
	
	loc = strstr( tmp, " " );
	if ( loc != 0 )
		loc[0] = 0;
	
	cmodes = tmp + 10;
	
	b_server_clear_cmodes( server, bModeTypeSetting );
	
	type = 0;
	while ( *cmodes != 0 )
	{
		if ( *cmodes == ',' )
			type++;
		else
		{
			if ( type == 0 )
				mflags = bModeHasParamAlways | bModeTypeList;
			else if ( type == 1 )
				mflags = bModeHasParamAlways | bModeRemember;
			else if ( type == 2 )
				mflags = bModeHasParamOnSet | bModeRemember;
			else
				mflags = bModeHasNoParams | bModeRemember;
			
			b_server_add_cmode( server, *cmodes, bModeTypeSetting | mflags );
		}
		
		cmodes++;
	}
	
	free( tmp );
	
	return 0;
}

BERS_MESSAGE( b_msg_chanmodeis )
{
	int a, b, c, d, e;
	char act, cch;
	BChatWindow *cw;
	char *tmp;
	char *modevictims = 0;
	
	cw = b_find_chat_by_dest( server, params[1] );
	
	if ( cw == 0 || cw->type != B_CMD_WINDOW_CHANNEL )
		return 0;
	
	d = 0;
	b = 3;
	
	for ( c = 0, act = '+'; c < strlen( params[2] ); c++ )
	{
		cch = params[2][c];
		
		if ( cch == '+' || cch == '-' )
		{
			act = cch;
			continue;
		}
		
		tmp = 0;
		if ( pcount > b )
			tmp = params[b];
		
		a = b_channel_mode_handle( server, cw, act, cch, params[1], tmp );
		b += a;
		
		if ( a > 0 )
		{
			e = d;
			d += strlen( tmp ) + 1;
			
			modevictims = (char *)realloc( modevictims, d+1 );
			
			if ( e == 0 )
				strcpy( modevictims, "" );
			else
				strcat( modevictims, " " );
			
			strcat( modevictims, tmp );
		}
	}
	
	if ( modevictims == 0 )
	{
		modevictims = (char *)malloc( 1 );
		strcpy( modevictims, "" );
	}
	
	lang_phrase_parse( lang_tmp_buf, 1024, "chan_all_modes", "mode", params[2], "victims", modevictims, 0 );
	b_chatwin_printf( cw, BTV_ModeChange, "%s", lang_tmp_buf );
	
	free( modevictims );
	
	return 0;
}

BERS_MESSAGE( b_msg_mode )
{
	char *sender = "NULL";
	BUserMask *from = (BUserMask *)window;
	int a, b, c, d, e;
	char act, cch;
	BChatWindow *cw;
	char *tmp;
	char *modevictims = 0;
	
	if ( flags & 4096 )
		sender = from->nickname;
	
	if ( pcount < 2 )
		return -1;
	
	if ( !strcasecmp( server->nickname, params[0] ) )
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "umode", "nick", sender, "mode", params[1], 0 );
		b_swindow_printf( server, BTV_ModeChange, "%s", lang_tmp_buf );
	}
	else
	{
		// channel modes.
		
		cw = b_find_chat_by_dest( server, params[0] );
		
		if ( cw == 0 || cw->type != B_CMD_WINDOW_CHANNEL )
			return 0;
		
		d = 0;
		b = 2;
		
		for ( c = 0, act = '+'; c < strlen( params[1] ); c++ )
		{
			cch = params[1][c];

			if ( cch == '+' || cch == '-' )
			{
				act = cch;
				continue;
			}
			
			tmp = 0;
			if ( pcount > b )
				tmp = params[b];
			
			a = b_channel_mode_handle( server, cw, act, cch, params[1], tmp );
			b += a;
			
			if ( a > 0 )
			{
				e = d;
				d += strlen( tmp ) + 1;
				
				modevictims = (char *)realloc( modevictims, d+1 );
				
				if ( e == 0 )
					strcpy( modevictims, "" );
				else
					strcat( modevictims, " " );
				
				strcat( modevictims, tmp );
			}
		}
		
		if ( modevictims == 0 )
		{
			modevictims = (char *)malloc( 1 );
			strcpy( modevictims, "" );
		}
		
		lang_phrase_parse( lang_tmp_buf, 1024, "mode", "nick", sender, "mode", params[1], "victims", modevictims, 0 );
		b_chatwin_printf( cw, BTV_ModeChange, "%s", lang_tmp_buf );
		
		free( modevictims );
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_nick )
{
	char *sender = "NULL";
	BUserMask *from = (BUserMask *)window;
	BChatWindow *cw;
	BUserStore *user;
	int a;
	char nickmsg[1024];
	node_t *n;
	
	if ( flags & 4096 )
		sender = from->nickname;
	
	if ( pcount < 1 )
		return -1;
	
	a = 0;
	
	if ( !strcasecmp( sender, server->nickname ) )
	{
		// we're the user! we've changed nicknames.
		strcpy( server->nickname, params[0] );
		b_server_update_title( server );
		a = 1;
	}
	
	if ( a == 1 )
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "younick", "nick", params[0], 0 );
	}
	else
	{
		lang_phrase_parse( lang_tmp_buf, 1024, "nick", "oldnick", from->nickname, "nick", params[0], 0 );
	}
	
	sprintf( nickmsg, "%s", lang_tmp_buf );
	
	LIST_FOREACH( n, server->chat_windows.head )
	{
		cw = (BChatWindow *)n->data;
		
		if ( cw->type == B_CMD_WINDOW_CHANNEL )
		{
			// channel window
			if ( ( user = b_chat_user_find_nick( cw, from->nickname ) ) != 0 )
			{
				c_btv_printf( cw->content, 0, BTV_UserNick, "%s", nickmsg );
				strcpy( user->nickname, params[0] );
				
				b_userstore_updated( cw, user, 1 );
				//b_update_userlist( cw );
			}
		}
		else
		{
			// query window
			if ( !strcasecmp( cw->dest, from->nickname ) )
			{
				// query to this user
				if ( b_get_option_bool( xidentity, "general", "opt_misc_nick_in_query" ) )
				{
					// write it here too
					c_btv_printf( cw->content, 0, BTV_UserNick, "%s", nickmsg );
				}
				
				if ( b_get_option_bool( xidentity, "general", "opt_misc_nick_update_query" ) )
				{
					// change this window's dest
					strcpy( cw->dest, params[0] );
					b_chat_update_title( cw );
				}
			}
		}
	}
	
	return 0;
}

BERS_MESSAGE( b_msg_unknowncmd )
{
	if ( pcount < 3 )
		return -1;
	
	b_window_printf( server, BTV_Error, "/%s: %s", params[1], params[2] );
	
	return 0;
}

BERS_MESSAGE( b_msg_writeparm1 )
{
	if ( pcount < 2 )
		return -1;
	
	b_window_printf( server, BTV_ServerWelcome, "%s", params[1] );
	
	return 0;
}

BERS_MESSAGE( b_msg_writeparm2 )
{
	if ( pcount < 3 )
		return -1;
	
	b_window_printf( server, BTV_ServerWelcome, "%s", params[2] );
	
	return 0;
}

BERS_MESSAGE( b_msg_writeparm2toactive )
{
	if ( pcount < 3 )
		return -1;
	
	b_window_printf( b_active_window( ), BTV_ModeChange, "%s", params[2] );
	
	return 0;
}

BERS_MESSAGE( b_msg_away )
{
	if ( pcount < 2 )
		return -1;
	
	b_window_printf( b_active_window( ), BTV_Info, "* %s", params[1] );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_311 )
{
	if ( pcount < 5 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "whois_info", "nick", params[1], "user", params[2], "host", params[3], "name", params[5], 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_314 )
{
	if ( pcount < 5 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "whois_was", "nick", params[1], "user", params[2], "host", params[3], "name", params[5], 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_is )
{
	if ( pcount < 3 )
		return -1;
	
	b_window_printf( b_active_window( ), BTV_Whois, "%s %s", params[1], params[2] );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_319 )
{
	if ( pcount < 3 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "whois_on", "nick", params[1], "what", params[2], 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_312 )
{
	if ( pcount < 4 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "whois_using", "nick", params[1], "server", params[2], "quote", params[3], 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_330 )
{
	if ( pcount < 4 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "whois_authed_as", "nick", params[1], "auth", params[2], 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_317 )
{
	int secsidle, signon;
	char *idle, *sign;
	char currtimes[256];
	time_t curtime;
	
	if ( pcount < 5 )
		return -1;
	
	secsidle = signon = 0;
	idle = sign = "unknown";
	
	secsidle = atoi( params[2] );
	signon = atoi( params[3] );
	
	curtime = signon;
	strftime( currtimes, 256, b_get_option_string( xidentity, "time", "opt_time_long_timestamp" ), localtime(&curtime) );
	
	lang_phrase_parse( lang_tmp_buf, 1024, "whois_idle", "nick", params[1], "idle", idle, "signon", currtimes, 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_whois_338 )
{
	if ( pcount < 4 )
		return -1;
	
	b_window_printf( b_active_window( ), BTV_Whois, "%s %s %s", params[1], params[3], params[2] );
	
	return 0;
}

BERS_MESSAGE( b_msg_invite_341 )
{
	if ( pcount < 3 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "youinvited", "nick", params[1], "chan", params[2], 0 );
	b_window_printf( b_active_window( ), BTV_Info, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_uaway )
{
	if ( pcount < 3 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "away", "nick", params[1], "reason", params[2], 0 );
	b_window_printf( b_active_window( ), BTV_Whois, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_time )
{
	if ( pcount < 3 )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "server-time", "time", params[2], "server", params[1], 0 );
	b_window_printf( b_active_window( ), BTV_Info, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_null )
{
	return 0;
}

BERS_MESSAGE( b_msg_checkprefix )
{
	char *prefix;
	char *tmp, *loc;
	
	prefix = strstr( origcmd, "PREFIX=" );
	
	if ( prefix == 0 )
		return 0;
	
	tmp = strdup( prefix );
	
	loc = strstr( tmp, " " );
	if ( loc != 0 )
		loc[0] = 0;
	
	prefix = tmp + 7;
	
	sscanf( prefix, "(%[^)])%s", b_irc_modes, b_irc_symbs );
	
	free( tmp );
	
	b_server_clear_cmodes( server, bModeTypePower );
	
	tmp = b_irc_modes;
	while ( *tmp )
	{
		b_server_add_cmode( server, *tmp, bModeTypePower );
		tmp++;
	}
	
	return 0;
}

/*
	// :nenolod!~nenolod@nenolod.developer.athemenet INVITE Theo :#priv_meeting
	b_register_message( "invite", &b_msg_invite, 0 );
	
	// :nenolod!~nenolod@nenolod.developer.athemenet WALLOPS :OPERWALL - what
	b_register_message( "wallops", &b_msg_wallops, 0 );
	
	// :irc.free2code.net 329 Theo #priv_meeting 1129514890
*/

BERS_MESSAGE( b_msg_createtime )
{
	BChatWindow *chatwin;
	char currtimes[256];
	time_t curtime;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[1] );
	
	if ( chatwin == NULL )
		return -1;
	
	curtime = atoi( params[2] );
	strftime( currtimes, 256, b_get_option_string( xidentity, "time", "opt_time_long_timestamp" ), localtime(&curtime) );
	
	lang_phrase_parse( lang_tmp_buf, 1024, "channel_created", "date", currtimes, 0 );
	b_chatwin_printf( chatwin, BTV_Topic, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_invite )
{
	BUserMask *from = (BUserMask *)window;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "channel_invite", "user", from->nickname, "chan", params[1], 0 );
	b_window_printf( b_active_window( ), BTV_Invite, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_wallops )
{
	BUserMask *from = (BUserMask *)window;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "wallops", "user", from->nickname, "msg", params[0], 0 );
	b_window_printf( server, BTV_Wallops, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_notopic )
{
	BChatWindow *chatwin;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[1] );
	
	if ( chatwin == NULL )
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, "channel_notopic", 0 );
	b_chatwin_printf( chatwin, BTV_Topic, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_chanurl )
{
	BChatWindow *chatwin;
	
	if ( pcount < 2 )
		return -1;
	
	chatwin = b_find_chat_by_dest( server, params[1] );
	
	if ( chatwin == NULL )
		return -1;
	
	// FIXME: Topic: Wrong colour?
	lang_phrase_parse( lang_tmp_buf, 1024, "channel_url", "url", params[2], 0 );
	b_chatwin_printf( chatwin, BTV_Topic, "%s", lang_tmp_buf );
	
	return 0;
}

BERS_MESSAGE( b_msg_stderror )
{
	//BUserMask *from = (BUserMask *)window;
	char *ermsg = "";
	
	     if ( !strcmp( command, "401" ) ) ermsg = "no_such_nick";
	else if ( !strcmp( command, "402" ) ) ermsg = "no_such_server";
	else if ( !strcmp( command, "403" ) ) ermsg = "no_such_channel";
	else if ( !strcmp( command, "404" ) ) ermsg = "cannot_send";
	else if ( !strcmp( command, "405" ) ) ermsg = "toomanychannels";
	else if ( !strcmp( command, "406" ) ) ermsg = "wasnosuchnick";
	else if ( !strcmp( command, "407" ) ) ermsg = "toomanytargets";
	//
	else if ( !strcmp( command, "471" ) ) ermsg = "cannot_join_l";
	else if ( !strcmp( command, "473" ) ) ermsg = "cannot_join_i";
	else if ( !strcmp( command, "474" ) ) ermsg = "cannot_join_b";
	else if ( !strcmp( command, "475" ) ) ermsg = "cannot_join_k";
	else if ( !strcmp( command, "482" ) ) ermsg = "not_chanop";
	else if ( !strcmp( command, "442" ) ) ermsg = "not_on_channel";
	else if ( !strcmp( command, "437" ) ) ermsg = "res_temp_unavail";
	else
		return -1;
	
	lang_phrase_parse( lang_tmp_buf, 1024, ermsg, "chan", params[1], "nick", params[1], "server", params[1], 0 );
	b_window_printf( b_active_window( ), BTV_Alert, "%s", lang_tmp_buf );
	
	return 0;
}

//
