#include "dfs.h"



int rfs_op_exists(char *path) {
	redisReply *rr;
	int ret = -ENOENT;

	rr = redisCommand(dfs.redis.rc, "EXISTS %s:%s", dfs.redis.ns, path);
	if(rr) {
		if (rr->type == REDIS_REPLY_INTEGER && rr->integer == 1) {
			ret = 0;
		}
		freeReplyObject(rr);
	}
	return ret;
}

int rfs_op_unlink(char *path) {
	redisReply *rr;
	int ret = -ENOENT;
	char * _dirname;

	ret = rfs_op_exists(path);
	if(ret < 0) return ret;

	rr = redisCommand(dfs.redis.rc, "DEL %s:%s", dfs.redis.ns, path);
	if(rr)
		freeReplyObject(rr);

	ret = 0;

	_dirname = xdirname(path);
	if(_dirname) {
		rr = redisCommand(dfs.redis.rc, "HDEL %s:%s:%s", dfs.redis.ns, _dirname, path);
		if(rr)
			freeReplyObject(rr);
	}

	return ret;
}


int rfs_op_fill_dir(void *buf, char *dir, fuse_fill_dir_t filler) {

	redisReply *rr;
	int ret = -EOPNOTSUPP, i;

	if(!buf || !dir || !filler) return ret;

	rr = redisCommand(dfs.redis.rc, "HGETALL %s:%s", dfs.redis.ns, dir);
	if(!rr) return ret;

	if(rr->type != REDIS_REPLY_ARRAY || rr->elements == 0) goto cleanup;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	for(i=0;i<rr->elements;i++) {
		struct stat st;
		redisReply * rr2 = redisCommand(dfs.redis.rc, "HGET %s:%s _stat", dfs.redis.ns, rr->element[i]->str);
		if(!rr2) continue;

		if(rr2->type != REDIS_REPLY_STRING || !rr2->str) { freeReplyObject(rr2); continue; }
			memcpy(&st, rr2->str, sizeof(st));
			xlog("rfs_op_fill_dir", "OK: %s\n", rr->element[i]->str);
			filler(buf, basename(strdup(&rr->element[i]->str[0])), &st, 0);
			freeReplyObject(rr2);
		}

		cleanup:
		freeReplyObject(rr);

		return 0;
}




int rfs_op_stat(char *path, struct stat *st, rfs_op_stat_t type) {
	redisReply *rr;
	int ret = -ENOENT;

	xlog("rfs_op_stat", "path=%s, st=%p\n", path, st);

	if(!path || !st) return -1;

	if(type == RFS_OP_STAT_SET) {
		rr = redisCommand(dfs.redis.rc, "HSET %s:%s _stat %b", dfs.redis.ns, path, st, sizeof(*st));
	}
	else {
		rr = redisCommand(dfs.redis.rc, "HGET %s:%s _stat", dfs.redis.ns, path);

		if(rr->type == REDIS_REPLY_STRING) {
			memcpy(st, rr->str, sizeof(*st));
			ret = 0;
		}
	}

	freeReplyObject(rr);

	return ret;
}


int rfs_op_read(char *path, char *buf, int len, int offset) {
	redisReply *rr;

	if(!path || !len) return -EIO;

	rr = redisCommand(dfs.redis.rc, "HGET %s:%s _data", dfs.redis.ns, path);

	if(rr) {
		if(rr->type == REDIS_REPLY_STRING) {

xlog("rfs_op_read", "%i %s\n", rr->len, rr->str);


			if(rr->len < len)
				len = rr->len;
				
			xlog("r2", "%i %i\n", len, offset);

			memcpy(buf, rr->str+offset, len);
		}
	}

	freeReplyObject(rr);

	return len;
}



int rfs_op_write(char *path, char *buf, int len, int offset, struct stat *st) {
	redisReply *rr;

	if(!path || !buf || len < 0 || offset < 0) return -EIO;

	rr = redisCommand(dfs.redis.rc, "HSET %s:%s _data %b", dfs.redis.ns, path, buf, len);

xlog("write3", "%i %i %i\n", len, offset, st->st_size);

	if(st) {
		if(st->st_size >= 0) {
			if(offset + len > st->st_size) {
				xlog("rfs_op_write", "%i %i\n", st->st_size, offset+len);
				st->st_size = offset + len;
				rfs_op_stat(path, st, RFS_OP_STAT_SET);
			}
		}
	}

	freeReplyObject(rr);

	return len;
}


int rfs_op_add_directory_to_directory(char *from, char *to) {
	return rfs_op_add_to_directory(from,to,RFS_DIR);
}

int rfs_op_add_file_to_directory(char *from, char *to) {
	return rfs_op_add_to_directory(from,to,RFS_FILE);
}

int rfs_op_add_link_to_directory(char *from, char *to) {
	return 0;
}

int rfs_op_add_dev_to_directory(char *from, char *to) {
	return 0;
}

/*
 * Add an rfs_node_type to an existing directory
 */
int rfs_op_add_to_directory(char *from, char *to, rfs_node_type type) {

	redisReply *rr;
	int ret = -EACCES;
	char *type_str;

	if(!from || !to) return ret;

	switch(type) {
		case RFS_DIR: {
			type_str = "d";
			break;
		}
		case RFS_FILE: {
			type_str = "f";
			break;
		}
		case RFS_LINK: {
			type_str = "l";
			break;
		}
		case RFS_DEV: {
			type_str = "dev";
			break;
		}
		default: {
			type_str = "unknown";
			break;
		}
	}

	rr = redisCommand(dfs.redis.rc, "HSET %s:%s %s %s", dfs.redis.ns, to, from, type_str);
	freeReplyObject(rr);

	return 0;
}



/*
 * These routines create a NEW rfs_node_type
 */

int rfs_op_create_directory(char * dir, mode_t mode) {
	return rfs_op_create(dir, mode, RFS_DIR);
}

int rfs_op_create_file(char *file, mode_t mode) {
	return rfs_op_create(file, mode, RFS_FILE);
}

int rfs_op_create_link(char *file, char *to, mode_t mode) {
	return 0;
}

int rfs_op_create_dev(char *file, int major, int minor, mode_t mode) {
	return 0;
}

int rfs_op_create(char * name, mode_t mode, rfs_node_type type) {

	redisReply * rr;
	struct stat st;
	int ret = -EACCES;
	char * _dirname;

	rr = redisCommand(dfs.redis.rc, "EXISTS %s:%s", dfs.redis.ns, name);
	if(!rr) return ret;

	if(rr->type != REDIS_REPLY_INTEGER) {
		freeReplyObject(rr);
		return ret;
	}

	if(rr->integer == 1) {
		return -EEXIST;
	}

	freeReplyObject(rr);

	rr = redisCommand(dfs.redis.rc, "HINCRBY %s:state inode 1", dfs.redis.ns);
	if(!rr) return ret;

	if(rr->type != REDIS_REPLY_INTEGER) {
		xlog("rfs_mkdir", "type=%i", rr->type);
		freeReplyObject(rr);
		return ret;
	}

	memset(&st,0,sizeof(st));
	st.st_ino = rr->integer;
	st.st_mode = mode;

	time(&st.st_atime);
	time(&st.st_mtime);
	time(&st.st_ctime);

	switch(type) {
		case RFS_DIR:
			st.st_mode |= S_IFDIR;
			break;
		case RFS_FILE:
			st.st_mode |= S_IFREG;
			break;
		case RFS_LINK: break;
		case RFS_DEV: break;
		default: break;
	}

	freeReplyObject(rr);

/*
	rr = redisCommand(dfs.redis.rc, "HSET %s:%s _stat %b", dfs.redis.ns, name, &st, sizeof(st));
	freeReplyObject(rr);
*/

	ret = rfs_op_stat(name, &st, RFS_OP_STAT_SET);

	_dirname = xdirname(name);
	if(!_dirname) return 0;

	xlog("create", "%s %s\n", name, _dirname);

	if(!strcmp(name,_dirname)) return 0;

	switch(type) {
		case RFS_DIR: {
			rfs_op_add_directory_to_directory(name, _dirname);
			break;
		}
		case RFS_FILE: {
			rfs_op_add_file_to_directory(name, _dirname);
			break;
		}
		case RFS_LINK: break;
		case RFS_DEV: break;
		default: break;
	}

	return 0;
}


void rfs_op_init_fs(void) {
	rfs_op_create_directory("/", 0755);
	rfs_op_create_directory("/state", 0777);
	return;
}
