CLARODIR=/usr/local/claro

CFLAGS=-I$(CLARODIR)/include -Wall -DENV_GTK `pkg-config --cflags gtk+-2.0`
OBJECTS=src/berscore.o \
src/btextview.o \
src/options.o \
src/plugins.o \
src/net/socket.o \
src/irc.o \
src/irc_cmd.o \
src/irc_msg.o \
src/xml/xml.o \
src/configdir.o \
src/lang.o \
src/dccwin.o \
src/dcc.o \
src/aboutwin.o \
src/urlwin.o \
src/profile.o \
src/netbrowser.o \
src/connectwin.o \
src/taskbar.o \
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
src/options/opt_misc.o \
src/options/opt_time.o \
src/options/opt_highlight.o

LDFLAGS=-L. -lclarogt

CFLAGS+=-g

all: bersirc

bersirc: $(OBJECTS)
	cp $(CLARODIR)/lib/libclarogt.so.1.0.1 .
	rm -rf libclarogt.so
	cp libclarogt.so.1.0.1 libclarogt.so
	gcc $(OBJECTS) $(LDFLAGS) -o bersirc

clean:
	rm -rf src/*.o
	rm -rf src/net/*.o
	rm -rf src/xml/*.o
