#ifndef ENODEB_H
#define ENODEB_H

#include "utils.h"
#include "packet.h"
#include "ran_data.h"
#include "udp_client.h"

struct EnodeB {
	UDPClient to_sgw;

	int tun_fd;
	string tun_name;
	Packet pkt;

	int status;
	
	string ue_ip;

	EnodeB();
	uint16_t generate_uteid(int);
	void attach_to_tun();
	void read_tun();
	void write_tun();
	void set_ue_ip();
	// void make_data();
	// void send_data();
	// void recv_data(int&);
	// void fill_tun_table(string&, TunData&);
	// void erase_tun_table(string&);
	~EnodeB();
};

#endif //ENODEB_H
