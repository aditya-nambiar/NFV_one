#include "sgw.h"

UDPServer g_sgw_server;
vector<pthread_t> g_tid;
int g_tcount;

SGW::SGW() {

}

uint16_t SGW::generate_cteid(int arg_ue_num){

	return (arg_ue_num);
}

uint16_t SGW::generate_uteid(int arg_ue_num){

	return (arg_ue_num);
}

void SGW::fill_pgw_details(int arg_ue_num){

	g_sgw_data[arg_ue_num].pgw_port = g_pgw_port;
	g_sgw_data[arg_ue_num].pgw_addr = g_pgw_addr;
}

void SGW::read_data(){

	pkt.clear_data();
	status = recvfrom(g_sgw_server.server_socket, pkt.data, BUF_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	pkt.curr_pos = 0;
}

void SGW::set_metadata(){

	pkt.copy_metadata(type, subtype, ue_num);
}

void SGW::store_create_session_data(){

	pkt.copy_data(g_sgw_data[ue_num].bearer_id);
	pkt.copy_data(g_sgw_data[ue_num].mme_cteid);
}

void SGW::create_session_req_to_pgw(){

	type = 1;
	subtype = 3;

	g_sgw_data[ue_num].sgw_cteid = generate_cteid(ue_num);
	g_sgw_data[ue_num].sgw_uteid = generate_uteid(ue_num);
	fill_pgw_details(ue_num);

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_sgw_data[ue_num].pgw_port, g_sgw_data[ue_num].pgw_addr.c_str());
	to_pgw.pkt.add_metadata(type, subtype, ue_num);
	to_pgw.pkt.add_data(g_sgw_data[ue_num].bearer_id);
	to_pgw.pkt.add_data(g_sgw_data[ue_num].sgw_cteid);
	to_pgw.pkt.add_data(g_sgw_data[ue_num].sgw_uteid);
	to_pgw.write_data();
}

void SGW::create_session_res_from_pgw(){
	char *ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	int len = INET_ADDRSTRLEN;

	to_pgw.read_data();
	to_pgw.close_client();
	to_pgw.pkt.copy_metadata(type, subtype, ue_num);
	to_pgw.pkt.copy_gtpc_hdr();
	to_pgw.pkt.copy_data(g_sgw_data[ue_num].pgw_cteid);
	to_pgw.pkt.copy_data(g_sgw_data[ue_num].pgw_uteid);
	to_pgw.pkt.copy_data(ip_addr, len);

	g_sgw_data[ue_num].ue_ip.assign(ip_addr);

	free(ip_addr);
}

void SGW::create_session_res_to_mme(){

	type = 1;
	subtype = 3;

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_gtpc_hdr(g_sgw_data[ue_num].mme_cteid);
	pkt.add_data(g_sgw_data[ue_num].sgw_cteid);

	status = sendto(g_sgw_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);	
}

void SGW::store_modify_session_data(){

	pkt.copy_gtpc_hdr();
	pkt.copy_data(g_sgw_data[ue_num].enodeb_uteid);

	g_sgw_data[ue_num].valid = true;
}

void SGW::modify_session_res_to_mme(){

	type = 1;
	subtype = 4;

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_gtpc_hdr(g_sgw_data[ue_num].mme_cteid);	
	pkt.add_data(g_sgw_data[ue_num].sgw_uteid);
	pkt.add_data(g_sgw_data[ue_num].ue_ip);

	status = sendto(g_sgw_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);
}

void SGW::delete_session_req_to_pgw(){


}

void SGW::delete_session_res_from_pgw(){


}

void SGW::delete_session_res_to_mme(){


}

void SGW::delete_session_data(){


}

SGW::~SGW() {

}