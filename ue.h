#ifndef UE_H
#define UE_H

#include "utils.h"
#include "packet.h"
#include "udp_client.h"
#include "ran_data.h"

struct UE {
	int num;
	int type;
	int subtype;
	
	unsigned long long autn_num;
	unsigned long long rand_num;
	unsigned long long autn_res;

	UDPClient to_mme;
	
	string reply;
	int status;
	bool success;

	UE(int&);
	unsigned long long generate_key(int&);
	void fill_mme_details();
	void start_mme_client();
	void send_attach_req();
	void recv_autn_req();
	unsigned long long set_autn_res();
	void send_autn_res();
	void recv_autn_check();
	void send_tun_data();
	void recv_tun_data();
	~UE();
};

#endif //UE_H
