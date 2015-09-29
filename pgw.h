#ifndef PGW_H
#define PGW_H

#include "utils.h"
#include "packet.h"
#include "udp_server.h"
#include "pgw_data.h"

#define MAX_IPS 200

struct PGW {
	struct sockaddr_in client_sock_addr;
	Packet pkt;

	int type;
	int subtype;
	int ue_num;

	int status;
	string reply;
	bool success;

	PGW();

	uint16_t generate_cteid(int);
	uint16_t generate_uteid(int);

	void read_data();
	void set_metadata();

	void store_create_session_data();
	void create_session_res_to_sgw();

	void delete_session_res_to_sgw();
	void delete_session_data();

	~PGW();
};

extern UDPServer g_pgw_server;
extern vector<pthread_t> g_tid;
extern int g_tcount;

extern vector<string> g_ip_table;

void generate_ip_table();

#endif //PGW_H
