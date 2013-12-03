#ifndef REDIS_H
#define REDIS_H

#include "dfs.h"

typedef struct redis {
	char * redis;
	char * redis_host;
	char * redis_port;
	char * redis_unix;
	char * ns;
	redisContext * rc;
} redis_t;

void redis_init_connection(void);
void redis_init_env(void);
void redis_init(void);
int redis_main(int, char **);

#endif
