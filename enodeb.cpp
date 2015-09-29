#include "enodeb.h"

EnodeB::EnodeB() {
	
}

uint16_t EnodeB::generate_uteid(int ue_num) {
	
	return ue_num;
}

void EnodeB::attach_to_tun() {	
	struct ifreq ifr;
	const char *dev = "/dev/net/tun";
	int flags;
	int status;

	tun_name = "tun1";
	flags = (IFF_TUN | IFF_NO_PI);
	tun_fd = open(dev , O_RDWR);
	report_error(tun_fd, "Opening /dev/net/tun");
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if (tun_name.size() != 0) {
		strncpy(ifr.ifr_name, tun_name.c_str(), IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void*)&ifr);
	if (status<0) {
		cout << "ioctl(TUNSETIFF)" << " " << errno << endl;
		close(tun_fd);
		exit(-1);
	}
	tun_name.assign(ifr.ifr_name);
	cout << "Enodeb attached to tun device - " << tun_name << endl;
}

void EnodeB::read_tun() {

	pkt.clear_data();
	status = read(tun_fd, pkt.data, BUF_SIZE);
	report_error(status);
	pkt.data_len = status;
}

void EnodeB::write_tun() {

	status = write(tun_fd, pkt.data, pkt.data_len);
	report_error(status);
}

void EnodeB::set_ue_ip() {
	struct ip *iphdr = allocate_ip_mem(IP_LEN);
	char *ip_addr = allocate_str_mem(INET_ADDRSTRLEN);

	memmove(iphdr, pkt.data, IP_LEN * sizeof(uint8_t));
	inet_ntop(AF_INET, &(iphdr->ip_src), ip_addr, INET_ADDRSTRLEN);
	ue_ip.assign(ip_addr);
	// cout << "Through tunnel: UE IP is " << ue_ip << endl;
	free(iphdr);
	free(ip_addr);
}

// void EnodeB::set_tun_data(bool &data_invalid) {
// 	string ue_ip_str;

// 	ue_ip_str.assign(ue_ip);
// 	if (g_tun_table.find(ue_ip_str) != g_tun_table.end()) {
// 		tun_data = g_tun_table[ue_ip_str];
// 		data_invalid = false;
// 	}
// 	else {
// 		cout << "Invalid data received!" << endl;
// 		data_invalid = true;
// 	}
// 	// cout << "Details fetched are: " << "UE IP - " << ue_ip_str << " SGW - port " << tun_data.sgw_port << " SGW addr " << tun_data.sgw_addr << endl;
// }

// void EnodeB::make_data() {

// 	pkt.fill_gtpu_hdr(tun_data.sgw_uteid);
// 	pkt.add_gtpu_hdr();
// }

// void EnodeB::send_data() {

// 	to_sgw[num].pkt.clear_data();
// 	to_sgw[num].pkt.fill_data(0, pkt.data_len, pkt.data);
// 	to_sgw[num].pkt.make_data_packet();
// 	to_sgw[num].write_data();
// }

// void EnodeB::recv_data(int &sgw_num) {

// 	to_sgw[sgw_num].read_data();
// 	pkt.clear_data();	
// 	pkt.fill_data(0, to_sgw[sgw_num].pkt.data_len, to_sgw[sgw_num].pkt.data);
// 	pkt.rem_gtpu_hdr();
// }

// void EnodeB::fill_tun_table(string &ue_ip_arg, TunData &tun_data_arg){

// 	g_tun_table[ue_ip_arg] = tun_data_arg;
// }

// void EnodeB::erase_tun_table(string &ue_ip_arg){

// 	// g_tun_table.erase(ue_ip_arg); /* This is to make the leftover data traffic to reach its intended destination  */
// }

EnodeB::~EnodeB() {

}
