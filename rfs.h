#ifndef RFS_H
#define RFS_H

typedef enum RFS_NODE_TYPE {
	RFS_DIR,
	RFS_FILE,
	RFS_LINK,
	RFS_DEV
} rfs_node_type;

typedef struct rfs {
} rfs_t;

void rfs_init(void);
int rfs_main(int, char **);

/*
 * Adding rfs_node_type's to directories
 */
int rfs_op_add_directory_to_directory(char *, char *);
int rfs_op_add_file_to_directory(char *, char *);
int rfs_op_add_link_to_directory(char *, char *);
int rfs_op_add_dev_to_directory(char *, char *);
int rfs_op_add_to_directory(char *, char *, rfs_node_type);

/*
 * Creating new rfs_node_type's
 */
int rfs_op_create_directory(char *, mode_t);
int rfs_op_create_file(char *, mode_t);
int rfs_op_create_link(char *, char *, mode_t);
int rfs_op_create_dev(char *, int, int, mode_t);
int rfs_op_create(char *, mode_t, rfs_node_type);

/*
 * Initialization routines
 */
void rfs_op_init_fs(void);

#endif
