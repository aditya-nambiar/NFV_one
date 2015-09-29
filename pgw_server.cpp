#include "pgw_server.h"

// void setup_tun() {

// 	system("sudo openvpn --rmtun --dev tun1");
// 	system("sudo openvpn --mktun --dev tun1");
// 	system("sudo ip link set tun1 up");
// 	system("sudo ip addr add 192.168.100.1/24 dev tun1");
// }

// void* monitor_traffic(void *arg) {
// 	PGWcMonitor pgwc_monitor;

// 	pgwc_monitor.attach_to_tun();
// 	pgwc_monitor.attach_to_sink();
// 	cout << "PGW Monitoring started " << endl;
// 	while (1) {
// 		pgwc_monitor.read_tun();
// 		pgwc_monitor.write_sink();		
// 	}
// }

void* process_traffic(void *arg) {
	PGW pgw;

	while(1){
		pgw.read_data();
		if(pgw.status == 0)
			continue;
		pgw.set_metadata();

		if(pgw.type == 1){	
			attach_process(pgw);
		}
		else if(pgw.type == 2){
			data_transfer(pgw);
		}
		else if(pgw.type == 3){
			detach_process(pgw);
		}
		else{
			cout << "Incorrect type - " << pgw.type << endl;
		}
	}
	return NULL;
}

void attach_process(PGW &pgw){
	
	if(pgw.subtype == 3){
		create_session(pgw);
	}
	else{
		cout << "Incorrect subtype for type 1 -> " << pgw.subtype << endl;
	}
}

void create_session(PGW &pgw){

	pgw.store_create_session_data();
	pgw.create_session_res_to_sgw();
}

void data_transfer(PGW &pgw){


}

void detach_process(PGW &pgw){

	if(pgw.subtype == 1){
		delete_session(pgw);
	}
	else{
		cout << "Incorrect subtype for type 3 -> " << pgw.subtype << endl;	
	}
}

void delete_session(PGW &pgw){

	pgw.delete_session_res_to_sgw();
	pgw.delete_session_data();
}

// void handle_udata(Server &pgw_server) {
// 	PGWu pgwu;
// 	fd_set read_set;
// 	int max_fd;
// 	int size;
// 	int i;
// 	int status;
// 	bool data_invalid;
	
// 	pgwu.configure_raw_client();
// 	pgwu.configure_server_for_sink();
// 	while (1) {
// 		FD_ZERO(&read_set);
// 		FD_SET(pgw_server.server_socket, &read_set); 
// 		FD_SET(pgwu.for_sink.server_socket, &read_set); 
// 		max_fd = max(pgw_server.server_socket, pgwu.for_sink.server_socket);
// 		status = select(max_fd + 1, &read_set, NULL, NULL, NULL);
// 		report_error(status, "Select-process failure\tTry again");		
// 		if (FD_ISSET(pgw_server.server_socket, &read_set)) {
// 			pgwu.recv_sgw(pgw_server);
// 			pgwu.send_raw_socket();		
// 		}
// 		if (FD_ISSET(pgwu.for_sink.server_socket, &read_set)) {
// 			pgwu.recv_sink();
// 			pgwu.set_ue_ip();
// 			pgwu.set_tun_udata(data_invalid);
// 			if (data_invalid)
// 				continue;
// 			pgwu.send_sgw(pgw_server);	
// 		}
// 	}
// }

void startup_pgw(char *argv[]){

	g_tcount = atoi(argv[1]);
	g_tid.resize(g_tcount);
	g_pgw_server.bind_server(g_pgw_port, g_pgw_addr.c_str());
	g_pgw_server.print_status("PGW");		
}

int main(int argc, char *argv[]) {
	int status;
	int i;

	check_server_usage(argc, argv);
	startup_pgw(argv);
	setup_pgw_data();
	generate_ip_table();

	for (i = 0; i < g_tcount; i++) {
		status = pthread_create(&g_tid[i], NULL, process_traffic, NULL);
		report_error(status);
	}
	for (i = 0; i < g_tcount; i++) {
		pthread_join(g_tid[i], NULL);
	}		

	free_pgw_data();
	return 0;

	// setup_tun();
	// status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	// report_error(status);	
}