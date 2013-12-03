#ifndef REDIS_OPS_H
#define REDIS_OPS_H

char * redis_dir_to_namespace(char *, char, char);
int redis_readdir_namespace(char *, void *, fuse_fill_dir_t);

#endif
