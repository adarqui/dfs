#include "dfs.h"


int redis_exists(char *key) {
	redisReply * rr;
	int ret = -ENOENT;

	rr = redisCommand(dfs.redis.rc, "EXISTS %s", key);
	if(rr) {
		if(rr->type == REDIS_REPLY_INTEGER && rr->integer == 1)
			ret = 0;

		freeReplyObject(rr);
	}

	return ret;
}



TCMAP * redis_keys_to_tcmap(char *keys) {
	TCMAP *map;
	redisReply *rr;
	int i;

	xlog("redis_keys_to_tcmap", "keys=%s\n", keys);

	rr = redisCommand(dfs.redis.rc, "KEYS %s", keys);
	if(!rr) return NULL;

	map = tcmapnew();
	if(!map) goto cleanup;

	for(i = 0; i < rr->elements; i++) {
		char * str = rr->element[i]->str;
		tcmapput(map, str, strlen(str), str, strlen(str));
	}

	cleanup:
	if(rr)
		freeReplyObject(rr);

	xlog("MAP", "%i\n", rr->elements);

	return map;
}

char * redis_dir_to_namespace(char *s, char cho, char chn) {

	char *s_base = s;

	xlog("redis_dir_to_namespace", "s=%s cho=%c chn=%c\n", s, cho, chn);
	if(!s) return NULL;

	while(*s) {
		xlog("WTF", "%c\n", *s);
		if(*s == cho)
			*s = chn;
		s++;
	}

	xlog("BUF", "%s\n", s);

	return s_base;
}


int redis_readdir_namespace(char * path, void *buf, fuse_fill_dir_t filler) {
	TCMAP *map;
	TCLISTDATUM *tcld;
	TCLIST * tcl;
	int ret = -ENOENT, i;
	struct stat st;
	char pathbuf[1024];

	xlog("redis_readdir_namespace", "path=%s\n", path);

	if(!buf || !filler) return ret;

	memset(pathbuf,0,sizeof(pathbuf));
	snprintf(pathbuf,sizeof(pathbuf)-1,"%s",path);
	map = redis_keys_to_tcmap(redis_dir_to_namespace(pathbuf,'/','*'));
	if(!map) return ret;

	tcl = tcmapvals(map);
	if(!tcl) return ret;

	xlog("redis_readdir_namespace", "anum=%i start=%i num=%i\n", tcl->anum, tcl->start, tcl->num);

	for(i=tcl->start;i<tcl->num;i++) {
		memset(&st,0,sizeof(st));
		tcld = &tcl->array[i];
		filler(buf, redis_dir_to_namespace((char *)tcld->ptr,'*','/'), &st, 0);
	}

	tclistclear(tcl);
	tcmapclear(map);
	
	return 0;
}
