/* DCC File Transfer Functions */

struct DCCData
{
	char *file;
	unsigned long fileSize;
	unsigned long position;
	unsigned long dpacketSize;
	unsigned long packetSize;
	unsigned long startTime; // stores the time, and with the previous variables, used to calculate necessary info
	int state; // (1 - 2) reserved for send; (3 - 4) reserved for receive
	int index;

	char *name;	
	unsigned long ip;
};

// Send
void dcc_send_init( CSocket *sock );
void dcc_send( CSocket *sock );

// Receive
void dcc_recv_init( CSocket *sock );
void dcc_recv( CSocket *sock );

void dcc_donothing( CSocket *sock );
void dcc_close( CSocket *sock );
