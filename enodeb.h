#ifndef ENODEB_H
#define ENODEB_H

#include "utils.h"
#include "packet.h"
#include "ran_data.h"
#include "udp_server.h"
#include "udp_client.h"

struct EnodeB {
	UDPServer enodeb_server;
	UDPClient to_sgw;

	struct sockaddr_in client_sock_addr;
	Packet pkt;

	int tun_fd;
	string tun_name;
	
	int type;
	int subtype;
	int ue_num;

	string ue_ip;
	
	int status;
	bool success;
	
	EnodeB();

	uint16_t generate_uteid(int);

	void startup_enodeb_server();

	void attach_to_tun();
	void read_tun();
	void write_tun();

	void set_ue_num();

	void send_sgw();
	void recv_sgw();

	void rem_headers();

	~EnodeB();
};

#endif //ENODEB_H
