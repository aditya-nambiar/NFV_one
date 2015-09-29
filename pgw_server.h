#ifndef PGW_SERVER_H
#define PGW_SERVER_H

#include "utils.h"
#include "packet.h"
#include "udp_server.h"
#include "udp_client.h"
#include "pgw_data.h"
#include "pgw.h"

void* process_traffic(void*);

void attach_process(PGW&);
void create_session(PGW&);

void data_transfer(PGW&);

void detach_process(PGW&);
void delete_session(PGW&);

void startup_pgw();

// void setup_tun();
// void* monitor_traffic(void*);

#endif //PGW_SERVER_H
