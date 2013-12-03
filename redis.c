#include "dfs.h"

static int redis_getattr(const char *path, struct stat *stbuf)
{

	int ret = -ENOENT;

	xlog("getattr", "path=%s stat={%W}\n", path, stbuf);

	if(path[0] == '/' && path[1] == '\0') {
		ret = lstat(path, stbuf);
		if (ret == -1)
			return -errno;
		return ret;
	}

	return 0;
}

static int redis_access(const char *path, int mask)
{

	xlog("access", "path=%s mask=%i\n", path, mask);

	return 0;
}


static int redis_readlink(const char *path, char *buf, size_t size)
{
	xlog("readlink", "path=%s buf=%p size=%ld\n", path, buf, size);

	return 0;
}


static int redis_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{

	xlog("readdir", "path=%s buf=%p filler=.. offset=%ld fi=%p\n", path, buf, offset, fi);

	redis_readdir_namespace((char *)path, buf, filler);

	return 0;
}

static int redis_mknod(const char *path, mode_t mode, dev_t rdev)
{

	xlog("mknod", "path=%s mode=%o rdev=%ld\n", path, mode, rdev);

	return 0;
}


static int redis_mkdir(const char *path, mode_t mode)
{
	xlog("mkdir", "path=%s mode=%o\n", path, mode);
	return 0;
}

static int redis_unlink(const char *path)
{
	xlog("unlink", "path=%s\n", path);
	return 0;
}

static int redis_rmdir(const char *path)
{
	xlog("rmdir", "path=%s\n", path);

	return 0;
}

static int redis_symlink(const char *from, const char *to)
{
	xlog("symlink", "from=%s to=%s\n", from, to);

	return 0;
}

static int redis_rename(const char *from, const char *to)
{
	xlog("rename", "from=%s to=%s\n", from, to);

	return 0;
}

static int redis_link(const char *from, const char *to)
{
	xlog("link", "from=%s to=%s\n", from, to);

	return 0;
}

static int redis_chmod(const char *path, mode_t mode)
{
	xlog("chmod", "path=%s mode=%o\n", path, mode);

	return 0;
}

static int redis_chown(const char *path, uid_t uid, gid_t gid)
{
	xlog("chown", "path=%s uid=%d gid=%d\n", path, uid, gid);

	return 0;
}

static int redis_truncate(const char *path, off_t size)
{
	xlog("truncate", "path=%s size=%ld\n", path, size);

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int redis_utimens(const char *path, const struct timespec ts[2])
{
	xlog("utimens", "path=%s ts=..\n", path);

	return 0;
}
#endif

static int redis_open(const char *path, struct fuse_file_info *fi)
{
	xlog("open", "path=%s fi=%p\n", path, fi);

	return 0;
}

static int redis_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	xlog("read", "path=%s buf=%p size=%ld offset=%ld fi=%p\n", path, buf, size, offset, fi);

	return 0;
}

static int redis_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	xlog("write", "path=%s buf=%p size=%ld offset=%ld fi=%p\n", path, buf, size, offset, fi);

	return 0;
}

static int redis_statfs(const char *path, struct statvfs *stbuf)
{
	xlog("statfs", "path=%s st=%p\n", path, stbuf);

	return 0;
}

static int redis_release(const char *path, struct fuse_file_info *fi)
{
	xlog("release", "path=%s fi=%p\n", path, fi);
	return 0;
}

static int redis_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	xlog("fsync", "path=%s isdatasync=%i fi=%p\n", path, isdatasync, fi);
	return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int redis_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	xlog("fallocate", "path=%s mode=%o offset=%ld length=%ld fi=%p\n", path, mode, offset, length, fi);

	return 0;
}
#endif

#ifdef HAVE_SETXATTR
static int redis_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	xlog("setxattr", "path=%s name=%s value=%s size=%ld flags=%i\n",
			path, name, value, size, flags);
	return 0;
}

static int redis_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	xlog("getxattr", "path=%s name=%s value=%s size=%ld\n", path, name, value, size);

	return res;
}

static int redis_listxattr(const char *path, char *list, size_t size)
{
	xlog("listxattr", "path=%s list=%s size=%ld\n", path, list, size);

	return res;
}

static int redis_removexattr(const char *path, const char *name)
{
	xlog("removexattr", "path=%s name=%s\n", path, name);

	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations redis_oper = {
	.getattr	= redis_getattr,
	.access		= redis_access,
	.readlink	= redis_readlink,
	.readdir	= redis_readdir,
	.mknod		= redis_mknod,
	.mkdir		= redis_mkdir,
	.symlink	= redis_symlink,
	.unlink		= redis_unlink,
	.rmdir		= redis_rmdir,
	.rename		= redis_rename,
	.link		= redis_link,
	.chmod		= redis_chmod,
	.chown		= redis_chown,
	.truncate	= redis_truncate,
#ifdef HAVE_UTIMENSAT
	.utimens	= redis_utimens,
#endif
	.open		= redis_open,
	.read		= redis_read,
	.write		= redis_write,
	.statfs		= redis_statfs,
	.release	= redis_release,
	.fsync		= redis_fsync,
#ifdef HAVE_POSIX_FALLOCATE
	.fallocate	= redis_fallocate,
#endif
#ifdef HAVE_SETXATTR
	.setxattr	= redis_setxattr,
	.getxattr	= redis_getxattr,
	.listxattr	= redis_listxattr,
	.removexattr	= redis_removexattr,
#endif
};



void redis_usage(char *s) {
	xlog("redis_usage", "Error:%s\n", s);
	exit(-1);
}


void redis_init_env(void) {

	dfs.redis.redis = getenv("REDIS");
	if(!dfs.redis.redis) redis_usage("Please specify a REDIS environment variable");

	if(dfs.redis.redis[0] == '/') {
		dfs.redis.redis_unix = dfs.redis.redis;
	}
	else {
		char * ptr_host, * ptr_port;
		ptr_host = ptr_port = NULL;
		ptr_host = strtok(dfs.redis.redis, ":");
		if(strstr(dfs.redis.redis, ":")) {
			ptr_port = strtok(NULL, "");
		}
		if(!ptr_port) ptr_port = "6379";
		dfs.redis.redis_host = ptr_host;
		dfs.redis.redis_port = ptr_port;
	}

	if(!dfs.redis.redis_host && !dfs.redis.redis_unix)
		redis_usage("Please specify a host or path using the REDIS environment variable");

	dfs.redis.ns = getenv("NS");
	if(!dfs.redis.ns) redis_usage("Please specify an NS environment variable");

return;
}


void redis_init_connection(void) {

	while(1) {
		if(dfs.redis.redis_unix) {
			dfs.redis.rc = redisConnectUnix(dfs.redis.redis_unix);
			if(dfs.redis.rc) return;
		} else {
			dfs.redis.rc = redisConnect(dfs.redis.redis_host, atoi(dfs.redis.redis_port));
			if(dfs.redis.rc) return;
		}
		sleep(1);
	}

	return;
}



void redis_init(void) {
	redis_init_env();
	redis_init_connection();
	redis_main(dfs.argc, dfs.argv);
	return;
}

int redis_main(int argc, char *argv[]) {
	return fuse_main(argc, argv, &redis_oper, NULL);
}
