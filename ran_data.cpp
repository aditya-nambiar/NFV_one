#include "ran_data.h"

struct RANData *g_ran_data;

unordered_map<string, int> g_ue_maptable;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

RANData::RANData(){

	valid = false;
}

RANData::~RANData(){


}

void setup_ran_data(){
	int i;

	g_ran_data = new struct RANData[MAX_CONNS];
	for(i=0;i<MAX_CONNS;i++)
		g_ran_data[i].ue_num = i;
}

void free_ran_data(){

	delete[] g_ran_data;
}