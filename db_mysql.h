#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

//(C) Operations: MySQL_DB functions
#include <mysql/mysql.h>
#include <mysql/my_global.h>

struct ConnDetails{
	const char *server;
	const char *user;
	const char *passwd;
	const char *database;
};

class MySql{
public:
	static ConnDetails details;
	MYSQL *conn;
	MYSQL_RES *result;
	
	MySql();
	void conn_setup();
	void perform_query(const char*);
	void fetch_result();
	void report_error(MYSQL*);
	~MySql();
};