#include "dfs.h"


static int rfs_getattr(const char *path, struct stat *stbuf)
{

	int ret = -ENOENT;

	xlog("getattr", "path=%s stat={%W}\n", path, stbuf);

	if(path[0] == '/' && path[1] == '\0') {
		ret = lstat(path, stbuf);
		if (ret == -1)
			return -errno;
		return ret;
	}

	return rfs_op_stat((char *)path, stbuf, RFS_OP_STAT_GET);
}

static int rfs_access(const char *path, int mask)
{

	xlog("access", "path=%s mask=%i\n", path, mask);

	return rfs_op_exists((char *)path);
}


static int rfs_readlink(const char *path, char *buf, size_t size)
{
	xlog("readlink", "path=%s buf=%p size=%ld\n", path, buf, size);

	return 0;
}


static int rfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{

	xlog("readdir", "path=%s buf=%p filler=.. offset=%ld fi=%p\n", path, buf, offset, fi);

	return rfs_op_fill_dir(buf, (char *)path, filler);
}

static int rfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	struct stat st;
	int ret;

	xlog("mknod", "path=%s mode=%o rdev=%ld\n", path, mode, rdev);

	ret = rfs_op_stat((char *)path, &st, RFS_OP_STAT_GET);
	if(ret >= 0) return -EEXIST;
	
	ret = rfs_op_create_file((char *)path, mode);
	if(ret < 0) return ret;

	return 0;
}


static int rfs_mkdir(const char *path, mode_t mode)
{
	int ret = -EEXIST;
	xlog("mkdir", "path=%s mode=%o\n", path, mode);
	ret = rfs_op_create_directory((char *)path, mode);
	if(ret < 0) return ret;
	return 0;
}

static int rfs_unlink(const char *path)
{

	xlog("unlink", "path=%s\n", path);

	return rfs_op_unlink((char *)path);
}

static int rfs_rmdir(const char *path)
{
	xlog("rmdir", "path=%s\n", path);

	return rfs_op_unlink((char *)path);
}

static int rfs_symlink(const char *from, const char *to)
{
	xlog("symlink", "from=%s to=%s\n", from, to);

	return 0;
}

static int rfs_rename(const char *from, const char *to)
{
	xlog("rename", "from=%s to=%s\n", from, to);

	return 0;
}

static int rfs_link(const char *from, const char *to)
{
	xlog("link", "from=%s to=%s\n", from, to);

	return 0;
}

static int rfs_chmod(const char *path, mode_t mode)
{
	xlog("chmod", "path=%s mode=%o\n", path, mode);

	return 0;
}

static int rfs_chown(const char *path, uid_t uid, gid_t gid)
{
	xlog("chown", "path=%s uid=%d gid=%d\n", path, uid, gid);

	return 0;
}

static int rfs_truncate(const char *path, off_t size)
{
	struct stat st;
	int ret;

	xlog("truncate", "path=%s size=%ld\n", path, size);

	ret = rfs_op_stat((char *)path, &st, RFS_OP_STAT_GET);
	if(ret <= 0) return -ENOENT;

	st.st_size = 0;

	return 0;
}

static int rfs_utimens(const char *path, const struct timespec ts[2])
{
	xlog("utimens", "path=%s ts=..\n", path);

	return 0;
}

static int rfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

	struct stat st;
	int ret;

	xlog("create", "path=%s mode=%d fi=%p\n", path, mode, fi);

	ret = rfs_op_stat((char *)path, &st, RFS_OP_STAT_GET);
	if(ret >= 0) return -EEXIST;

	ret = rfs_op_create_file((char *)path, mode);
	if(ret < 0) return ret;

	return 0;
}


static int rfs_open(const char *path, struct fuse_file_info *fi)
{
	xlog("open", "path=%s fi=%p\n", path, fi);

	return 0;
}

static int rfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	xlog("read", "path=%s buf=%p size=%ld offset=%ld fi=%p\n", path, buf, size, offset, fi);

	return rfs_op_read((char *)path, buf, size, offset);
}

static int rfs_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{

	struct stat st;
	int n = 0, ret;

	xlog("write", "path=%s buf=%p size=%ld offset=%ld fi=%p\n", path, buf, size, offset, fi);

	ret = rfs_op_stat((char *)path, &st, RFS_OP_STAT_GET);
	xlog("write2", "%i\n", ret);
	if(ret < 0) return -EIO;

	n = rfs_op_write((char *)path, (char *)buf, size, offset, &st);

	return n;
}

static int rfs_statfs(const char *path, struct statvfs *stbuf)
{
	xlog("statfs", "path=%s st=%p\n", path, stbuf);

	return 0;
}

static int rfs_release(const char *path, struct fuse_file_info *fi)
{
	xlog("release", "path=%s fi=%p\n", path, fi);
	return 0;
}

static int rfs_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	xlog("fsync", "path=%s isdatasync=%i fi=%p\n", path, isdatasync, fi);
	return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int rfs_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	xlog("fallocate", "path=%s mode=%o offset=%ld length=%ld fi=%p\n", path, mode, offset, length, fi);

	return 0;
}
#endif

#ifdef HAVE_SETXATTR
static int rfs_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	xlog("setxattr", "path=%s name=%s value=%s size=%ld flags=%i\n",
			path, name, value, size, flags);
	return 0;
}

static int rfs_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	xlog("getxattr", "path=%s name=%s value=%s size=%ld\n", path, name, value, size);

	return res;
}

static int rfs_listxattr(const char *path, char *list, size_t size)
{
	xlog("listxattr", "path=%s list=%s size=%ld\n", path, list, size);

	return res;
}

static int rfs_removexattr(const char *path, const char *name)
{
	xlog("removexattr", "path=%s name=%s\n", path, name);

	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations rfs_oper = {
	.getattr	= rfs_getattr,
	.access		= rfs_access,
	.readlink	= rfs_readlink,
	.readdir	= rfs_readdir,
	.mknod		= rfs_mknod,
	.mkdir		= rfs_mkdir,
	.symlink	= rfs_symlink,
	.unlink		= rfs_unlink,
	.rmdir		= rfs_rmdir,
	.rename		= rfs_rename,
	.link		= rfs_link,
	.chmod		= rfs_chmod,
	.chown		= rfs_chown,
	.truncate	= rfs_truncate,
	.utimens	= rfs_utimens,
	.create 	= rfs_create,
	.open		= rfs_open,
	.read		= rfs_read,
	.write		= rfs_write,
	.statfs		= rfs_statfs,
	.release	= rfs_release,
	.fsync		= rfs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
	.fallocate	= rfs_fallocate,
#endif
#ifdef HAVE_SETXATTR
	.setxattr	= rfs_setxattr,
	.getxattr	= rfs_getxattr,
	.listxattr	= rfs_listxattr,
	.removexattr	= rfs_removexattr,
#endif
};

void rfs_init(void) {
	redis_init_env();
	redis_init_connection();
	rfs_op_init_fs();
	rfs_main(dfs.argc, dfs.argv);
	return;
}

int rfs_main(int argc, char *argv[]) {
	return fuse_main(argc, argv, &rfs_oper, NULL);
}
