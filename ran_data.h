#ifndef RAN_DATA_H
#define RAN_DATA_H

#include "utils.h"

struct RANData{
	bool valid;

	int ue_num;
	unsigned long long key;
	unsigned long long imsi;
	unsigned long long msisdn;
	string ue_ip;

	uint16_t enodeb_uteid;

	int mme_port;
	string mme_addr;

	int sgw_port;
	string sgw_addr;
	uint16_t sgw_uteid;

	RANData();

	~RANData();
};

extern struct RANData *g_ran_data;

extern unordered_map<string, int> g_ue_maptable; // Data race cannot occur because of usage of Mutex lock
extern pthread_mutex_t g_lock; // To lock g_ue_maptable

void setup_ran_data();
void free_ran_data();

#endif //RAN_DATA_H
