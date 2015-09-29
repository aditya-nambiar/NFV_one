#ifndef PGW_DATA_H
#define PGW_DATA_H

#include "utils.h"

struct PGWData{
	bool valid;

	int ue_num;
	int bearer_id;
	string ue_ip;

	uint16_t sgw_cteid;
	uint16_t sgw_uteid;
	
	uint16_t pgw_cteid;
	uint16_t pgw_uteid;

	PGWData();

	~PGWData();
};

extern struct PGWData *g_pgw_data;

void setup_pgw_data();
void free_pgw_data();

#endif //PGW_DATA_H
