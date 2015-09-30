#include "mme.h"

UDPServer g_mme_server;
vector<pthread_t> g_tid;
int g_tcount;

MME::MME() {

}

uint16_t MME::generate_cteid(int arg_ue_num){

	return (arg_ue_num);
}

int MME::generate_bearer_id(int arg_ue_num){

	return (arg_ue_num);
}

void MME::fill_sgw_details(int arg_ue_num){

	g_mme_data[arg_ue_num].sgw_port = g_sgw1_port;
	g_mme_data[arg_ue_num].sgw_addr = g_sgw1_addr;
}

void MME::fill_pgw_details(int arg_ue_num){

	g_mme_data[arg_ue_num].pgw_port = g_pgw_port;
	g_mme_data[arg_ue_num].pgw_addr = g_pgw_addr;	
}

void MME::read_data(){

	pkt.clear_data();
	status = recvfrom(g_mme_server.server_socket, pkt.data, BUF_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	pkt.curr_pos = 0;
}

void MME::set_metadata(){

	pkt.copy_metadata(type, subtype, ue_num);
}

void MME::fetch_ue_data(){
	type = 1;
	subtype = 1;

	pkt.copy_data(imsi);
	pkt.copy_data(msisdn);
	to_hss.bind_client();
	to_hss.fill_server_details(g_hss_port, g_hss_addr.c_str());
	to_hss.pkt.add_metadata(type, subtype, ue_num);
	to_hss.pkt.add_data(imsi);
	to_hss.pkt.add_data(msisdn);
	to_hss.write_data();
	cout<<"Waiting to read data for UE - "<<ue_num<<endl;
	to_hss.read_data();
	to_hss.close_client();
}

void MME::store_ue_data(){

	to_hss.pkt.copy_metadata(type, subtype, ue_num);
	to_hss.pkt.copy_data(autn_num);
	to_hss.pkt.copy_data(rand_num);
	to_hss.pkt.copy_data(g_mme_data[ue_num].autn_xres);
}

void MME::send_autn_req(){

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_data(autn_num);
	pkt.add_data(rand_num);
	
	status = sendto(g_mme_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);
}

void MME::process_autn(){

	pkt.copy_data(autn_res);
	if(g_mme_data[ue_num].autn_xres == autn_res){
		reply = "OK";
		cout << "Authentication has been successful for UE - " << ue_num << endl;
		success = 1;
	}
	else{
		reply = "FAILURE";
		cout << "Authentication has not been successful for UE - " << ue_num << endl;
		success = 0;
	}
	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_data(reply);
	
	status = sendto(g_mme_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);		
}

void MME::create_session_req_to_sgw(){
	
	type = 1;
	subtype = 3;

	g_mme_data[ue_num].bearer_id = generate_bearer_id(ue_num);
	g_mme_data[ue_num].mme_cteid = generate_cteid(ue_num);
	fill_sgw_details(ue_num);
	fill_pgw_details(ue_num);
	
	to_sgw.bind_client();
	to_sgw.fill_server_details(g_mme_data[ue_num].sgw_port, g_mme_data[ue_num].sgw_addr.c_str());
	to_sgw.pkt.add_metadata(type, subtype, ue_num);
	to_sgw.pkt.add_data(g_mme_data[ue_num].bearer_id);
	to_sgw.pkt.add_data(g_mme_data[ue_num].mme_cteid);
	to_sgw.write_data();
}

void MME::create_session_res_from_sgw(){

	to_sgw.read_data();
	to_sgw.close_client();
	to_sgw.pkt.copy_metadata(type, subtype, ue_num);
	to_sgw.pkt.copy_gtpc_hdr();
	to_sgw.pkt.copy_data(g_mme_data[ue_num].sgw_cteid);
}

void MME::store_enodeb_data(){

	pkt.copy_data(g_mme_data[ue_num].enodeb_uteid);
}

void MME::modify_session_req_to_sgw(){
	
	type = 1;
	subtype = 4;

	to_sgw.bind_client();
	to_sgw.fill_server_details(g_mme_data[ue_num].sgw_port, g_mme_data[ue_num].sgw_addr.c_str());
	to_sgw.pkt.add_metadata(type, subtype, ue_num);
	to_sgw.pkt.add_data(g_mme_data[ue_num].enodeb_uteid);
	to_sgw.pkt.add_gtpc_hdr(g_mme_data[ue_num].sgw_cteid);
	to_sgw.write_data();
}

void MME::modify_session_res_from_sgw(){
	char *ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	int len = INET_ADDRSTRLEN;

	to_sgw.read_data();
	to_sgw.close_client();
	to_sgw.pkt.copy_metadata(type, subtype, ue_num);
	to_sgw.pkt.copy_gtpc_hdr();
	to_sgw.pkt.copy_data(g_mme_data[ue_num].sgw_uteid);
	to_sgw.pkt.copy_data(ip_addr, len);
	
	g_mme_data[ue_num].ue_ip.assign(ip_addr);
	g_mme_data[ue_num].valid = true;

	free(ip_addr);
}

void MME::send_attach_res(){

	type = 1;
	subtype = 4;

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_data(g_mme_data[ue_num].sgw_uteid);
	pkt.add_data(g_mme_data[ue_num].ue_ip);
	
	status = sendto(g_mme_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);			

	cout << "Tunnel is formed successfully from UE to PGW for UE - " << ue_num << endl;
}

void MME::delete_session_req_to_sgw(){

	type = 3;
	subtype = 1;

	to_sgw.bind_client();
	to_sgw.fill_server_details(g_mme_data[ue_num].sgw_port, g_mme_data[ue_num].sgw_addr.c_str());
	to_sgw.pkt.add_metadata(type, subtype, ue_num);
	to_sgw.pkt.add_gtpc_hdr(g_mme_data[ue_num].sgw_cteid);
	to_sgw.write_data();
}

void MME::delete_session_res_from_sgw(){
	char *res = allocate_str_mem(BUF_SIZE);
	int len;

	to_sgw.read_data();
	to_sgw.close_client();
	to_sgw.pkt.copy_metadata(type, subtype, ue_num);
	to_sgw.pkt.copy_gtpc_hdr();
	len = (to_sgw.pkt.data_len - to_sgw.pkt.curr_pos);
	to_sgw.pkt.copy_data(res, len);

	reply.assign(res);
	cout << "This is the delete session reply - " << reply << " , for UE - " << ue_num << endl;
	if(reply == "OK"){
		success = 1;
		cout << "Detach request has been successful at MME for UE  - " << ue_num << endl;
	}
	else{
		success = 0;
		cout << "Detach request has not been successful at MME for UE  - " << ue_num << endl;		
	}

	free(res);
}

void MME::send_detach_res(){

	type = 3;
	subtype = 1;

	if(success == 1){
		reply = "OK";
	}
	else{
		reply = "FAILURE";
	}

	pkt.add_metadata(type, subtype, ue_num);
	pkt.add_data(reply);

	status = sendto(g_mme_server.server_socket, pkt.data, pkt.data_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);			
}	

void MME::delete_session_data(){

	g_mme_data[ue_num].valid = false;
}

MME::~MME() {

}
