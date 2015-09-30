#include "pgw.h"

UDPServer g_pgw_server;
vector<pthread_t> g_tid;
pthread_t g_downlink_tid;
int g_tcount;

vector<string> g_ip_table;

PGW::PGW() {


}

uint16_t PGW::generate_cteid(int arg_ue_num){

	return (arg_ue_num);
}

uint16_t PGW::generate_uteid(int arg_ue_num){

	return (arg_ue_num);
}

void PGW::read_data(){

	pkt.clear_data();
	status = recvfrom(g_pgw_server.server_socket, pkt.data, BUF_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	pkt.curr_pos = 0;
}

void PGW::set_metadata(){

	pkt.copy_metadata(type, subtype, ue_num);
}

void PGW::set_sgw_details(int arg_ue_num){

	g_pgw_data[arg_ue_num].sgw_port = g_sgw1_port;
	g_pgw_data[arg_ue_num].sgw_addr = g_sgw1_addr;
}

void PGW::store_create_session_data(){

	pkt.copy_data(g_pgw_data[ue_num].bearer_id);
	pkt.copy_data(g_pgw_data[ue_num].sgw_cteid);
	pkt.copy_data(g_pgw_data[ue_num].sgw_uteid);
}

void PGW::create_session_res_to_sgw(){

	type = 1;
	subtype = 3;

	g_pgw_data[ue_num].pgw_cteid = generate_cteid(ue_num);
	g_pgw_data[ue_num].pgw_uteid = generate_uteid(ue_num);
	g_pgw_data[ue_num].ue_ip = g_ip_table[ue_num];
	g_pgw_data[ue_num].valid = true;

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_gtpc_hdr(g_pgw_data[ue_num].sgw_cteid);
	pkt.add_data(g_pgw_data[ue_num].pgw_cteid);
	pkt.add_data(g_pgw_data[ue_num].pgw_uteid);
	pkt.add_data(g_pgw_data[ue_num].ue_ip);

	status = sendto(g_pgw_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);		

	cout << "Tunnel is formed successfully from UE to PGW for UE - " << ue_num << endl;
}

void PGW::add_map_entry(){

	if(g_pgw_data[ue_num].valid == true){
		string key = g_pgw_data[ue_num].ue_ip;
		int value = g_pgw_data[ue_num].ue_num;

		status = pthread_mutex_lock(&g_lock);
		report_error(status, "Error in thread locking");

		g_ue_maptable[key] = value;

		status = pthread_mutex_unlock(&g_lock);
		report_error(status, "Error in thread unlocking");
	}
}

void PGW::make_uplink_data(){

	int len;

	pkt.copy_gtpu_hdr();
	len = (pkt.data_len - pkt.curr_pos);
	to_public_sink.pkt.clear_data();
	to_public_sink.pkt.add_data(pkt.data + pkt.curr_pos, len);	
}

void PGW::send_public_sink(){

	to_public_sink.bind_client();
	to_public_sink.set_server_details(g_public_sink_port, g_public_sink_addr.c_str());
	to_public_sink.write_data();
	to_public_sink.close_client();
}

void PGW::delete_session_res_to_sgw(){

	success = 1;

	type = 3;
	subtype = 1;
	reply = "OK";

	cout << "Delete session request has been successful at PGW for UE - " << ue_num << endl;	
	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_gtpc_hdr(g_pgw_data[ue_num].sgw_cteid);
	pkt.add_data(reply);

	status = sendto(g_pgw_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);		
}

void PGW::delete_session_data(){

	g_pgw_data[ue_num].valid = false;

	if(g_pgw_data[ue_num].valid == false){
		string key = g_pgw_data[ue_num].ue_ip;

		status = pthread_mutex_lock(&g_lock);
		report_error(status, "Error in thread locking");

		// g_ue_maptable.erase(key); // Commented this because data validity is checked using valid bit while sending data

		status = pthread_mutex_unlock(&g_lock);
		report_error(status, "Error in thread unlocking");
	}
}

PGW::~PGW() {


}

void generate_ip_table() {
	string prefix;
	string ip;
	int i;
	
	g_ip_table.resize(MAX_IPS);
	
	prefix = "192.168.100.";
	for (i = 0; i < MAX_IPS; i++) {
		ip = prefix + to_string(i+3);
		g_ip_table[i] = ip;
	}
}
