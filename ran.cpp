#include "ran.h"

EnodeB g_enodeb;
int g_total_connections;
double g_req_duration;
time_t g_start_time;

void setup_tun() {

	system("sudo openvpn --rmtun --dev tun1");
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg) {
	// fd_set read_set;
	// int max_fd;
	// int size;
	// int i;
	// int status;
	// bool data_invalid;

	// g_enodeb.attach_to_tun();
	// while (1) {
	// 	FD_ZERO(&read_set);
	// 	FD_SET(g_enodeb.tun_fd, &read_set); 
	// 	max_fd = g_enodeb.tun_fd;
	// 	size = g_enodeb.pos;
	// 	for (i = 0; i < size; i++) {
	// 		FD_SET(g_enodeb.to_sgw[i].ue_socket, &read_set); 
	// 		max_fd = max(max_fd, g_enodeb.to_sgw[i].ue_socket);
	// 	}
	// 	status = select(max_fd + 1, &read_set, NULL, NULL, NULL);
	// 	report_error(status, "Select-process failure\tTry again");		
	// 	if (FD_ISSET(g_enodeb.tun_fd, &read_set)) {
	// 		g_enodeb.read_tun();
	// 		g_enodeb.set_ue_ip();
	// 		g_enodeb.set_tun_data(data_invalid);
	// 		if (data_invalid)
	// 			continue;
	// 		g_enodeb.set_sgw_num();
	// 		g_enodeb.make_data();
	// 		g_enodeb.send_data();
	// 	}
	// 	for (i = 0; i < size; i++) {
	// 		if (FD_ISSET(g_enodeb.to_sgw[i].ue_socket, &read_set)) {
	// 			g_enodeb.recv_data(i);
	// 			g_enodeb.write_tun();
	// 		}
	// 	}
	// }
}

void* generate_traffic(void *arg) {
	int ue_num;
	bool time_exceeded;
	
	ue_num = *((int*)arg);
	time_exceeded = false;
	UE ue(ue_num);
	ue.fill_mme_details();
	ue.start_mme_client();
	while (1) {
		authenticate(ue);
		if(!ue.success)
			continue;
		setup_tunnel(ue);
		send_traffic();
		detach(ue);
		// sleep(1);
		time_check(g_start_time, g_req_duration, time_exceeded);
		if (time_exceeded) {
			break;
		}
	}
	return NULL;
}

void authenticate(UE &ue) {

	ue.send_attach_req();
	ue.recv_autn_req();
	ue.set_autn_res();
	ue.send_autn_res();
	ue.recv_autn_check();
}

void setup_tunnel(UE &ue){

	set_tun_data(ue);
	ue.send_tun_data();
	ue.recv_tun_data();
}

void set_tun_data(UE &ue){

	g_ran_data[ue.num].enodeb_uteid = g_enodeb.generate_uteid(ue.num);
}

void send_traffic() {

}

void detach(UE &ue) {

}

void startup_ran(char *argv[], vector<int> &ue_num, vector<pthread_t> &tid) {

	g_start_time = time(0);
	g_total_connections = atoi(argv[1]);
	g_req_duration = atof(argv[2]);
	ue_num.resize(g_total_connections);
	tid.resize(g_total_connections);
}

int main(int argc, char *argv[]) {
	int i;
	int status;
	vector<int> ue_num;
	pthread_t mon_tid;
	vector<pthread_t> tid;

	check_client_usage(argc, argv);
	startup_ran(argv, ue_num, tid);
	setup_ran_data();
	setup_tun();
	status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	report_error(status);
	for (i = 0; i < g_total_connections; i++) {
		ue_num[i] = i;
		status = pthread_create(&tid[i], NULL, generate_traffic, &ue_num[i]);
		report_error(status);
	}
	for (i = 0; i < g_total_connections; i++) {
		pthread_join(tid[i],NULL);
	}
	free_ran_data();
	cout << "Requested duration has ended. Finishing the program." << endl;
	return 0;
}
