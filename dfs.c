#include "dfs.h"


void main_usage(char *s) {
	xlog("main_usage", "Error:Reason=%s\n", s);
	exit(-1);
}

void main_init(void) {
	umask(0);
	hook_printf();
	log_init();
	return;
}

int main(int argc, char **argv)
{

	char * type;

	main_init();

	xlog("main", "dfs: Initialized\n");

	type = getenv("DFS_TYPE");
	if(!type) {
		main_usage("Specify DFS_TYPE=");
	}

	dfs.argc = argc;
	dfs.argv = argv;

	if(!strcasecmp(type, "redis")) {
		redis_init();
	} else if(!strcasecmp(type, "rfs")) {
		rfs_init();
	} else if(!strcasecmp(type, "null")) {
		null_init();
	}

	return 0;
}
