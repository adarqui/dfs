#ifndef RFS_OPS_H
#define RFS_OPS_H

typedef enum RFS_OP_STAT_T {
	RFS_OP_STAT_GET,
	RFS_OP_STAT_SET
} rfs_op_stat_t;

void rfs_op_init_fs(void);

int rfs_op_exists(char *);
int rfs_op_unlink(char *);
int rfs_op_write(char *, char *, int, int, struct stat *);
int rfs_op_stat(char *, struct stat *, rfs_op_stat_t);
int rfs_op_fill_dir(void *, char *, fuse_fill_dir_t);
int rfs_op_read(char *, char *, int, int);


#endif
