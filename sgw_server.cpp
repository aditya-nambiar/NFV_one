#include "sgw_server.h"

void* process_traffic(void *arg) {
	SGW sgw;

	while(1){
		sgw.read_data();
		if(sgw.status == 0)
			continue;
		sgw.set_metadata();

		if(sgw.type == 1){	
			attach_process(sgw);
		}
		else if(sgw.type == 2){
			data_transfer(sgw);
		}
		else if(sgw.type == 3){
			detach_process(sgw);
		}
		else{
			cout << "Incorrect type - " << sgw.type << endl;
		}
	}
	return NULL;
}

void attach_process(SGW &sgw){

	if(sgw.subtype == 3){
		create_session(sgw);
	}
	else if(sgw.subtype == 4){
		modify_session(sgw);
	}
	else{
		cout << "Incorrect subtype for type - 1 -> " << sgw.subtype << endl;
	}
}

void create_session(SGW &sgw){

	sgw.store_create_session_data();
	sgw.create_session_req_to_pgw();
	sgw.create_session_res_from_pgw();
	sgw.create_session_res_to_mme();
}

void modify_session(SGW &sgw){

	sgw.store_modify_session_data();
	sgw.modify_session_res_to_mme();
}

void data_transfer(SGW &sgw){


}

void detach_process(SGW &sgw){

	if(sgw.subtype == 1){
		delete_session(sgw);
	}
	else{
		cout << "Incorrect subtype for type - 3 -> " << sgw.subtype << endl;
	}
}

void delete_session(SGW &sgw){

	sgw.delete_session_req_to_pgw();
	sgw.delete_session_res_from_pgw();
	sgw.delete_session_res_to_mme();
	if(sgw.success == 1){
		sgw.delete_session_data();
	}
}

// void handle_udata(Server &sgw_server) {
// 	SGWu sgwu;
// 	fd_set read_set;
// 	int max_fd;
// 	int size;
// 	int i;
// 	int status;
// 	bool data_invalid;

// 	while (1) {
// 		FD_ZERO(&read_set);
// 		FD_SET(sgw_server.server_socket, &read_set); 
// 		max_fd = sgw_server.server_socket;
// 		size = sgwu.pos;
// 		for (i = 0; i < size; i++) {
// 			FD_SET(sgwu.to_pgw[i].client_socket, &read_set); 
// 			max_fd = max(max_fd, sgwu.to_pgw[i].client_socket);
// 		}
// 		status = select(max_fd + 1, &read_set, NULL, NULL, NULL);
// 		report_error(status, "Select-process failure\tTry again");		
// 		if (FD_ISSET(sgw_server.server_socket, &read_set)) {
// 			sgwu.recv_enodeb(sgw_server);
// 			sgwu.set_uteid();
// 			sgwu.set_tun_udata(data_invalid);
// 			if (data_invalid)
// 				continue;
// 			sgwu.set_pgw_num();
// 			sgwu.make_data_pgw();
// 			sgwu.send_pgw();
// 		}
// 		for (i = 0; i < size; i++) {
// 			if (FD_ISSET(sgwu.to_pgw[i].client_socket, &read_set)) {
// 				sgwu.recv_pgw(i);
// 				sgwu.set_uteid();
// 				sgwu.set_tun_udata(data_invalid);
// 				if (data_invalid)
// 					continue;				
// 				sgwu.make_data_enodeb();
// 				sgwu.send_enodeb(sgw_server);
// 			}
// 		}
// 	}
// }

void startup_sgw(char *argv[]){

	g_tcount = atoi(argv[1]);
	g_tid.resize(g_tcount);
	g_sgw_server.bind_server(g_sgw1_port, g_sgw1_addr.c_str());
	g_sgw_server.print_status("SGW");	
}

int main(int argc, char *argv[]) {
	int status;
	int i;
		
	check_server_usage(argc, argv);
	startup_sgw(argv);
	setup_sgw_data();	

	for (i = 0; i < g_tcount; i++) {
		status = pthread_create(&g_tid[i], NULL, process_traffic, NULL);
		report_error(status);
	}
	for (i = 0; i < g_tcount; i++) {
		pthread_join(g_tid[i], NULL);
	}		

	free_sgw_data();
	return 0;
}