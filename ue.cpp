#include "ue.h"

UE::UE(int &arg_ue_num) {

	ue_num = arg_ue_num;
	g_ran_data[ue_num].key = generate_key(ue_num);
	g_ran_data[ue_num].imsi = g_ran_data[ue_num].key * 1000;
	g_ran_data[ue_num].msisdn = 9000000000 + g_ran_data[ue_num].key;
}

unsigned long long UE::generate_key(int &ue_num) {

	return ue_num;
}

void UE::set_mme_details(){

	g_ran_data[ue_num].mme_port = g_mme_port;
	g_ran_data[ue_num].mme_addr = g_mme_addr;
}

void UE::startup_mme_client(){

	to_mme.bind_client();
	to_mme.set_server_details(g_ran_data[ue_num].mme_port, g_ran_data[ue_num].mme_addr.c_str());
}

void UE::send_attach_req(){
	
	type = 1;
	subtype = 1;

	to_mme.pkt.add_metadata(type, subtype, ue_num);
	to_mme.pkt.add_data(g_ran_data[ue_num].imsi);
	to_mme.pkt.add_data(g_ran_data[ue_num].msisdn);
	to_mme.write_data();
}

void UE::recv_autn_req(){

	cout<<"Waiting to read data_1 : UE - "<<ue_num<<endl;
	to_mme.read_data(success);
	if(!success){
		return;
	}

	to_mme.pkt.copy_metadata(type, subtype, ue_num);
	to_mme.pkt.copy_data(autn_num);
	to_mme.pkt.copy_data(rand_num);
	cout << "AUTN_NUM is " << autn_num << ". RAND_NUM is " << rand_num << ". UE - " << ue_num << endl;
}

unsigned long long UE::set_autn_res() {

	autn_res = (autn_num * g_ran_data[ue_num].key) + (rand_num * (g_ran_data[ue_num].key + 1));
}

void UE::send_autn_res(){
	
	type = 1;
	subtype = 2;

	to_mme.pkt.add_metadata(type, subtype, ue_num);
	to_mme.pkt.add_data(autn_res);
	to_mme.write_data();
}

void UE::recv_autn_check(){
	char *res = allocate_str_mem(BUF_SIZE);
	int len;

	cout<<"Waiting to read data_2 : UE - "<<ue_num<<endl;
	to_mme.read_data(success);
	if(!success){
		return;
	}

	to_mme.pkt.copy_metadata(type, subtype, ue_num);
	len = (to_mme.pkt.data_len - to_mme.pkt.curr_pos);
	to_mme.pkt.copy_data(res, len);
	cout<<"Type is "<<type<<". Subtype is "<<subtype<<". UE ue_num is "<<ue_num<<endl;
	cout << "This is the message - " << res << endl;
	reply.assign(res);
	if (reply == "OK"){
		print_message("Authentication Successful for UE - ", ue_num);
		success = 1;
	}
	else {
		cout << "Authentication is not successful for UE - " << ue_num << endl;
		success = 0;
	}
	free(res);
}

void UE::send_tun_data(){
	
	type = 1;
	subtype = 3;

	to_mme.pkt.add_metadata(type, subtype, ue_num);
	to_mme.pkt.add_data(g_ran_data[ue_num].enodeb_uteid);
	to_mme.write_data();
}

void UE::set_sgw_details(){

	g_ran_data[ue_num].sgw_port = g_sgw1_port;
	g_ran_data[ue_num].sgw_addr = g_sgw1_addr;
}

void UE::recv_tun_data(){
	char *ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	int len = INET_ADDRSTRLEN;

	to_mme.read_data(success);
	if(!success){
		return;
	}

	to_mme.pkt.copy_metadata(type, subtype, ue_num);
	to_mme.pkt.copy_data(g_ran_data[ue_num].sgw_uteid);
	to_mme.pkt.copy_data(ip_addr, len);
	
	g_ran_data[ue_num].ue_ip.assign(ip_addr);
	g_ran_data[ue_num].valid = true;

	cout << "Tunnel is formed successfully from UE to PGW for UE - " << ue_num << endl;

	free(ip_addr);
}

void UE::add_map_entry(){

	if(g_ran_data[ue_num].valid == true){
		string key = g_ran_data[ue_num].ue_ip;
		int value = g_ran_data[ue_num].ue_num;

		status = pthread_mutex_lock(&g_lock);
		report_error(status, "Error in thread locking");

		g_ue_maptable[key] = value;

		status = pthread_mutex_unlock(&g_lock);
		report_error(status, "Error in thread unlocking");
	}
}

void UE::turn_up_interface(){
	string cmd;

	interface_name = "eth0:" + to_string(ue_num);

	cmd = "sudo ifconfig " + interface_name + " " + g_ran_data[ue_num].ue_ip + "/16";
	cout << cmd << endl;

	system(cmd.c_str());
	cout << "Interface successfully created for UE - " << ue_num << endl;
}

void UE::setup_sink(){

	sink_port = (ue_num + 55000);
	sink_addr = g_private_sink_addr;
}

void UE::send_iperf3_traffic(){
	string cmd;
	string rate;
	string mtu;
	string dur;

	rate = " -b 1M";
	mtu = " -M 500";
	dur = " -t 1";

	cmd = "iperf3 -B " + g_ran_data[ue_num].ue_ip + " -c " + sink_addr + " -p " + to_string(sink_port) + rate + mtu + dur;
	cout << cmd << endl;
	
	system(cmd.c_str());
	cout << "Iperf3 Traffic has been successfully sent for UE - " << ue_num << endl;
}

void UE::turn_down_interface(){
	string cmd;

	cmd = "sudo ifconfig " + interface_name + " down";
	cout << cmd << endl;

	// system(cmd.c_str());
	cout << "Interface turned down for UE - " << ue_num << endl;	
}

void UE::send_detach_req(){

	type = 3;
	subtype = 1;

	to_mme.pkt.add_metadata(type, subtype, ue_num);
	to_mme.write_data();
}

void UE::recv_detach_res(){
	char *res = allocate_str_mem(BUF_SIZE);
	int len;

	to_mme.read_data(success);
	if(!success){
		return;
	}

	to_mme.pkt.copy_metadata(type, subtype, ue_num);
	len = (to_mme.pkt.data_len - to_mme.pkt.curr_pos);
	to_mme.pkt.copy_data(res, len);

	reply.assign(res);
	cout << "This is the detach session reply - " << reply << " , for UE - " << ue_num << endl;
	if(reply == "OK"){
		success = 1;
		cout << "Detach has been successful at UE - " << ue_num << endl;
	}
	else{
		success = 0;
		cout << "Detach has not been successful at UE - " << ue_num << endl;	
	}

	free(res);
}

void UE::delete_session_data(){

	g_ran_data[ue_num].valid = false;

	if(g_ran_data[ue_num].valid == false){
		string key = g_ran_data[ue_num].ue_ip;

		status = pthread_mutex_lock(&g_lock);
		report_error(status, "Error in thread locking");

		// g_ue_maptable.erase(key); // Commented this because data validity is checked using valid bit while sending data

		status = pthread_mutex_unlock(&g_lock);
		report_error(status, "Error in thread unlocking");
	}
}

UE::~UE(){

}