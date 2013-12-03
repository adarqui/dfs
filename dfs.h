#ifndef DFS_H
#define DFS_H

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#define _GNU_SOURCE
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include <tcutil.h>
#include <hiredis/hiredis.h>

#include "misc.h"
#include "log.h"
#include "printf.h"
#include "redis.h"
#include "redis_ops.h"
#include "rfs.h"
#include "rfs_ops.h"
#include "null.h"

/*
typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
				const struct stat *stbuf, off_t off);
typedef struct fuse_dirhandle *fuse_dirh_t;
typedef int (*fuse_dirfil_t) (fuse_dirh_t h, const char *name, int type,
			      ino_t ino);
*/

typedef struct dfs_operations {
	int (*getattr) (const char *, struct stat *);
	int (*readlink) (const char *, char *, size_t);
	int (*getdir) (const char *, fuse_dirh_t, fuse_dirfil_t);
	int (*mknod) (const char *, mode_t, dev_t);
	int (*mkdir) (const char *, mode_t);
	int (*unlink) (const char *);
	int (*rmdir) (const char *);
	int (*symlink) (const char *, const char *);
	int (*rename) (const char *, const char *);
	int (*link) (const char *, const char *);
	int (*chmod) (const char *, mode_t);
	int (*chown) (const char *, uid_t, gid_t);
	int (*truncate) (const char *, off_t);
	int (*utime) (const char *, struct utimbuf *);
	int (*open) (const char *, struct fuse_file_info *);
	int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);
	int (*write) (const char *, const char *, size_t, off_t, struct fuse_file_info *);
	int (*statfs) (const char *, struct statvfs *);
	int (*flush) (const char *, struct fuse_file_info *);
	int (*release) (const char *, struct fuse_file_info *);
	int (*fsync) (const char *, int, struct fuse_file_info *);
	int (*setxattr) (const char *, const char *, const char *, size_t, int);
	int (*getxattr) (const char *, const char *, char *, size_t);
	int (*listxattr) (const char *, char *, size_t);
	int (*removexattr) (const char *, const char *);
	int (*opendir) (const char *, struct fuse_file_info *);
	int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
	int (*releasedir) (const char *, struct fuse_file_info *);
	int (*fsyncdir) (const char *, int, struct fuse_file_info *);
	void (*destroy) (void *);
	int (*access) (const char *, int);
	int (*create) (const char *, mode_t, struct fuse_file_info *);
	int (*fgetattr) (const char *, struct stat *, struct fuse_file_info *);
	int (*lock) (const char *, struct fuse_file_info *, int cmd, struct flock *);
	int (*utimens) (const char *, const struct timespec tv[2]);
	int (*bmap) (const char *, size_t blocksize, uint64_t *idx);
	int (*ioctl) (const char *, int cmd, void *arg, struct fuse_file_info *, unsigned int flags, void *data);
	int (*poll) (const char *, struct fuse_file_info *, struct fuse_pollhandle *ph, unsigned *reventsp);
	int (*write_buf) (const char *, struct fuse_bufvec *buf, off_t off, struct fuse_file_info *);
	int (*read_buf) (const char *, struct fuse_bufvec **bufp, size_t size, off_t off, struct fuse_file_info *);
	int (*flock) (const char *, struct fuse_file_info *, int op);
	int (*fallocate) (const char *, int, off_t, off_t, struct fuse_file_info *);
} dfs_ops_t;

/*
int fs_getattr(struct fuse_fs *fs, const char *path, struct stat *buf);
int fs_fgetattr(struct fuse_fs *fs, const char *path, struct stat *buf,
		     struct fuse_file_info *fi);
int fs_rename(struct fuse_fs *fs, const char *oldpath,
		   const char *newpath);
int fs_unlink(struct fuse_fs *fs, const char *path);
int fs_rmdir(struct fuse_fs *fs, const char *path);
int fs_symlink(struct fuse_fs *fs, const char *linkname,
		    const char *path);
int fs_link(struct fuse_fs *fs, const char *oldpath, const char *newpath);
int fs_release(struct fuse_fs *fs,	 const char *path,
		    struct fuse_file_info *fi);
int fs_open(struct fuse_fs *fs, const char *path,
		 struct fuse_file_info *fi);
int fs_read(struct fuse_fs *fs, const char *path, char *buf, size_t size,
		 off_t off, struct fuse_file_info *fi);
int fs_read_buf(struct fuse_fs *fs, const char *path,
		     struct fuse_bufvec **bufp, size_t size, off_t off,
		     struct fuse_file_info *fi);
int fs_write(struct fuse_fs *fs, const char *path, const char *buf,
		  size_t size, off_t off, struct fuse_file_info *fi);
int fs_write_buf(struct fuse_fs *fs, const char *path,
		      struct fuse_bufvec *buf, off_t off,
		      struct fuse_file_info *fi);
int fs_fsync(struct fuse_fs *fs, const char *path, int datasync,
		  struct fuse_file_info *fi);
int fs_flush(struct fuse_fs *fs, const char *path,
		  struct fuse_file_info *fi);
int fs_statfs(struct fuse_fs *fs, const char *path, struct statvfs *buf);
int fs_opendir(struct fuse_fs *fs, const char *path,
		    struct fuse_file_info *fi);
int fs_readdir(struct fuse_fs *fs, const char *path, void *buf,
		    fuse_fill_dir_t filler, off_t off,
		    struct fuse_file_info *fi);
int fs_fsyncdir(struct fuse_fs *fs, const char *path, int datasync,
		     struct fuse_file_info *fi);
int fs_releasedir(struct fuse_fs *fs, const char *path,
		       struct fuse_file_info *fi);
int fs_create(struct fuse_fs *fs, const char *path, mode_t mode,
		   struct fuse_file_info *fi);
int fs_lock(struct fuse_fs *fs, const char *path,
		 struct fuse_file_info *fi, int cmd, struct flock *lock);
int fs_flock(struct fuse_fs *fs, const char *path,
		  struct fuse_file_info *fi, int op);
int fs_chmod(struct fuse_fs *fs, const char *path, mode_t mode);
int fs_chown(struct fuse_fs *fs, const char *path, uid_t uid, gid_t gid);
int fs_truncate(struct fuse_fs *fs, const char *path, off_t size);
int fs_ftruncate(struct fuse_fs *fs, const char *path, off_t size,
		      struct fuse_file_info *fi);
int fs_utimens(struct fuse_fs *fs, const char *path,
		    const struct timespec tv[2]);
int fs_access(struct fuse_fs *fs, const char *path, int mask);
int fs_readlink(struct fuse_fs *fs, const char *path, char *buf,
		     size_t len);
int fs_mknod(struct fuse_fs *fs, const char *path, mode_t mode,
		  dev_t rdev);
int fs_mkdir(struct fuse_fs *fs, const char *path, mode_t mode);
int fs_setxattr(struct fuse_fs *fs, const char *path, const char *name,
		     const char *value, size_t size, int flags);
int fs_getxattr(struct fuse_fs *fs, const char *path, const char *name,
		     char *value, size_t size);
int fs_listxattr(struct fuse_fs *fs, const char *path, char *list,
		      size_t size);
int fs_removexattr(struct fuse_fs *fs, const char *path,
			const char *name);
int fs_bmap(struct fuse_fs *fs, const char *path, size_t blocksize,
		 uint64_t *idx);
int fs_ioctl(struct fuse_fs *fs, const char *path, int cmd, void *arg,
		  struct fuse_file_info *fi, unsigned int flags, void *data);
int fs_poll(struct fuse_fs *fs, const char *path,
		 struct fuse_file_info *fi, struct fuse_pollhandle *ph,
		 unsigned *reventsp);
int fs_fallocate(struct fuse_fs *fs, const char *path, int mode,
		 off_t offset, off_t length, struct fuse_file_info *fi);
void fs_init(struct fuse_fs *fs, struct fuse_conn_info *conn);
void fs_destroy(struct fuse_fs *fs);
*/


typedef enum DFS_CREATE_TYPE {
	DFS_CREATE_DIR,
	DFS_CREATE_FILE
} dfs_create_type_t;



typedef struct DFS_INSTANCE {
	char name[32];
	struct stat st;
	TCMAP *map;
} dfs_i_t;

typedef struct DFS {
	TCMAP * map;
	int argc;
	char **argv;
	redis_t redis;
	rfs_t rfs;
	null_t null;
} dfs_t;

dfs_t dfs;

void main_usage(char *);
void main_init(void);

#endif
