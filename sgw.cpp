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

void SGW::set_pgw_details(int arg_ue_num){

	g_sgw_data[arg_ue_num].pgw_port = g_pgw_port;
	g_sgw_data[arg_ue_num].pgw_addr = g_pgw_addr;
}

void SGW::create_session_req_to_pgw(){

	type = 1;
	subtype = 3;

	g_sgw_data[ue_num].sgw_cteid = generate_cteid(ue_num);
	g_sgw_data[ue_num].sgw_uteid = generate_uteid(ue_num);

	to_pgw.bind_client();
	to_pgw.set_server_details(g_sgw_data[ue_num].pgw_port, g_sgw_data[ue_num].pgw_addr.c_str());
	to_pgw.pkt.add_metadata(type, subtype, ue_num);
	to_pgw.pkt.add_data(g_sgw_data[ue_num].bearer_id);
	to_pgw.pkt.add_data(g_sgw_data[ue_num].sgw_cteid);
	to_pgw.pkt.add_data(g_sgw_data[ue_num].sgw_uteid);
	to_pgw.write_data();
}

void SGW::create_session_res_from_pgw(){
	char *ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	int len = INET_ADDRSTRLEN;

	to_pgw.read_data(success);
	to_pgw.close_client();
	if(!success){
		return;
	}
	to_pgw.pkt.copy_metadata(type, subtype, ue_num);
	to_pgw.pkt.copy_gtpc_hdr();
	to_pgw.pkt.copy_data(g_sgw_data[ue_num].pgw_cteid);
	to_pgw.pkt.copy_data(g_sgw_data[ue_num].pgw_uteid);
	to_pgw.pkt.copy_data(ip_addr, len);

	g_sgw_data[ue_num].ue_ip.assign(ip_addr);

	cout << "Received IP address is **" << ip_addr << "** for UE - " << ue_num << endl;

	free(ip_addr);
}

void SGW::create_session_res_to_mme(){

	type = 1;
	subtype = 3;

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_gtpc_hdr(g_sgw_data[ue_num].mme_cteid);
	pkt.add_data(g_sgw_data[ue_num].sgw_cteid);

	g_sgw_server.write_data(client_sock_addr, pkt);
}

void SGW::set_enodeb_details(int arg_ue_num){

	g_sgw_data[arg_ue_num].enodeb_port = g_enodeb_port;
	g_sgw_data[arg_ue_num].enodeb_addr = g_enodeb_addr;
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

	g_sgw_server.write_data(client_sock_addr, pkt);

	cout << "Tunnel is formed successfully from UE to PGW for UE - " << ue_num << endl;	
}

void SGW::make_uplink_data(){
	int len;

	type = 2;
	subtype = 1;

	pkt.copy_gtpu_hdr();
	len = (pkt.data_len - pkt.curr_pos);
	to_pgw.pkt.add_metadata(type, subtype, ue_num);
	to_pgw.pkt.add_gtpu_hdr(g_sgw_data[ue_num].pgw_uteid);
	to_pgw.pkt.add_data(pkt.data + pkt.curr_pos, len);
}

void SGW::send_pgw(){

	to_pgw.bind_client();
	to_pgw.set_server_details(g_sgw_data[ue_num].pgw_port, g_sgw_data[ue_num].pgw_addr.c_str());
	to_pgw.write_data();
	to_pgw.close_client();
}

void SGW::make_downlink_data(){
	int len;

	type = 2;
	subtype = 2;

	pkt.copy_gtpu_hdr();
	len = (pkt.data_len - pkt.curr_pos);
	to_enodeb.pkt.add_metadata(type, subtype, ue_num);
	to_enodeb.pkt.add_gtpu_hdr(g_sgw_data[ue_num].enodeb_uteid);
	to_enodeb.pkt.add_data(pkt.data + pkt.curr_pos, len);
}

void SGW::send_enodeb(){

	to_enodeb.bind_client();
	to_enodeb.set_server_details(g_sgw_data[ue_num].enodeb_port, g_sgw_data[ue_num].enodeb_addr.c_str());
	to_enodeb.write_data();
	to_enodeb.close_client();
}

void SGW::delete_session_req_to_pgw(){

	type = 3;
	subtype = 1;

	to_pgw.bind_client();
	to_pgw.set_server_details(g_sgw_data[ue_num].pgw_port, g_sgw_data[ue_num].pgw_addr.c_str());
	to_pgw.pkt.add_metadata(type, subtype, ue_num);
	to_pgw.pkt.add_gtpc_hdr(g_sgw_data[ue_num].pgw_cteid);
	to_pgw.write_data();
}

void SGW::delete_session_res_from_pgw(){
	char *res = allocate_str_mem(BUF_SIZE);
	int len;

	to_pgw.read_data(success);
	to_pgw.close_client();
	if(!success){
		return;
	}
	to_pgw.pkt.copy_metadata(type, subtype, ue_num);
	to_pgw.pkt.copy_gtpc_hdr();
	len = (to_pgw.pkt.data_len - to_pgw.pkt.curr_pos);
	to_pgw.pkt.copy_data(res, len);

	reply.assign(res);
	cout << "This is the delete session reply - " << reply << " , for UE - " << ue_num << endl;
	if(reply == "OK"){
		success = 1;
		cout << "Delete session request has been successful at SGW for UE - " << ue_num << endl;
	}
	else{
		success = 0;
		cout << "Delete session request has not been successful at SGW for UE - " << ue_num << endl;
	}

	free(res);
}

void SGW::delete_session_res_to_mme(){

	type = 3;
	subtype = 1;

	if(success == 1){
		reply = "OK";
	}
	else{
		reply = "FAILURE";
	}

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_gtpc_hdr(g_sgw_data[ue_num].mme_cteid);
	pkt.add_data(reply);

	g_sgw_server.write_data(client_sock_addr, pkt);
}

void SGW::delete_session_data(){

	g_sgw_data[ue_num].valid = false;
}

SGW::~SGW() {

}