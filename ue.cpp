#include "ue.h"

UE::UE(int &ue_num) {

	num = ue_num;
	g_ran_data[num].key = generate_key(num);
	g_ran_data[num].imsi = g_ran_data[num].key * 1000;
	g_ran_data[num].msisdn = 9000000000 + g_ran_data[num].key;
}

unsigned long long UE::generate_key(int &ue_num) {

	return ue_num;
}

void UE::fill_mme_details(){

	g_ran_data[num].mme_port = g_mme_port;
	g_ran_data[num].mme_addr = g_mme_addr;
}

void UE::start_mme_client(){

	to_mme.bind_client();
	to_mme.fill_server_details(g_ran_data[num].mme_port, g_ran_data[num].mme_addr.c_str());
}

void UE::send_attach_req(){
	
	type = 1;
	subtype = 1;

	to_mme.pkt.add_metadata(type, subtype, num);
	to_mme.pkt.add_data(g_ran_data[num].imsi);
	to_mme.pkt.add_data(g_ran_data[num].msisdn);
	to_mme.write_data();
}

void UE::recv_autn_req(){

	cout<<"Waiting to read data_1 : UE - "<<num<<endl;
	to_mme.read_data();
	to_mme.pkt.copy_metadata(type, subtype, num);
	to_mme.pkt.copy_data(autn_num);
	to_mme.pkt.copy_data(rand_num);
	cout << "AUTN_NUM is " << autn_num << ". RAND_NUM is " << rand_num << ". UE - " << num << endl;
}

unsigned long long UE::set_autn_res() {

	autn_res = (autn_num * g_ran_data[num].key) + (rand_num * (g_ran_data[num].key + 1));
}

void UE::send_autn_res(){
	
	type = 1;
	subtype = 2;

	to_mme.pkt.add_metadata(type, subtype, num);
	to_mme.pkt.add_data(autn_res);
	to_mme.write_data();
}

void UE::recv_autn_check(){
	char *res = allocate_str_mem(BUF_SIZE);
	int len;

	cout<<"Waiting to read data_2 : UE - "<<num<<endl;
	to_mme.read_data();
	to_mme.pkt.copy_metadata(type, subtype, num);
	len = (to_mme.pkt.data_len - to_mme.pkt.curr_pos);
	to_mme.pkt.copy_data(res, len);
	cout<<"Type is "<<type<<". Subtype is "<<subtype<<". UE num is "<<num<<endl;
	cout << "This is the message - " << res << endl;
	reply.assign(res);
	if (reply == "OK"){
		print_message("Authentication Successful for UE - ", num);
		success = 1;
	}
	else {
		cout << "Authentication is not successful for UE - " << num << endl;
		success = 0;
	}
	free(res);
}

void UE::send_tun_data(){
	
	type = 1;
	subtype = 3;

	to_mme.pkt.add_metadata(type, subtype, num);
	to_mme.pkt.add_data(g_ran_data[num].enodeb_uteid);
	to_mme.write_data();
}

void UE::recv_tun_data(){
	char *ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	int len = INET_ADDRSTRLEN;

	to_mme.read_data();
	to_mme.pkt.copy_metadata(type, subtype, num);
	to_mme.pkt.copy_data(g_ran_data[num].sgw_uteid);
	to_mme.pkt.copy_data(ip_addr, len);
	
	g_ran_data[num].ue_ip.assign(ip_addr);
	g_ran_data[num].valid = true;

	free(ip_addr);
}

UE::~UE(){

}