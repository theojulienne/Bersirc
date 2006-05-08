CLARODIR=../claro

CFLAGS=-I$(CLARODIR)/src -Wall -DENV_GTK -D_NIX `pkg-config --cflags gtk+-2.0`
OBJECTS=src/core.o \
src/ircview.o \
src/irc.o \
src/commands.o \
src/messages.o \
src/favorites.o \
src/menus.o \
src/server.o \
src/chat.o \
src/connection.o \
src/windows.o \
src/configdir.o \
src/lang.o \
src/taskbar.o \
src/plugins.o \
src/aboutwin.o \
src/profile.o \
src/netbrowser.o \
src/connectwin.o \
src/dcc.o \
src/dccwin.o \
src/urlwin.o \
src/joinwin.o \
src/autowin.o \
src/net/socket.o \
src/xml/xml.o \
src/options.o \
src/options/opt_general.o \
src/options/opt_connections.o \
src/options/opt_irc_servers.o \
src/options/opt_firewall_ip.o \
src/options/opt_visual.o \
src/options/opt_assorted.o \
src/options/opt_taskbar.o \
src/options/opt_nicklist.o \
src/options/opt_fonts.o \
src/options/opt_colour.o \
src/options/opt_text.o \
src/options/opt_highlight.o \
src/options/opt_misc.o \
src/options/opt_time.o \

LDFLAGS=-L. -lclaro-base -lclaro-graphics -L$(CLARODIR)/build

CFLAGS+=-g

all: bersirc

bersirc: $(OBJECTS) .
	gcc $(OBJECTS) $(LDFLAGS) -o bersirc

clean:
	rm -rf src/*.o
	rm -rf src/net/*.o
	rm -rf src/xml/*.o
