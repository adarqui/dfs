FLAGS=  # -DDEBUG

all:
	gcc -Wall -O3 dfs.c rfs.c rfs_ops.c redis.c redis_ops.c null.c printf.c log.c misc.c -D__USE_GNU -ltokyocabinet -lhiredis $(FLAG) `pkg-config fuse --cflags --libs` -o dfs

clean:
	rm -f dfs


mount_redis:
	make mount
	DFS_TYPE="redis" NS=":" REDIS="127.0.0.1:6379" ./dfs -o allow_other /tmp/dfs

mount_rfs:
	make mount
	DFS_TYPE="rfs" NS="rfs" REDIS="127.0.0.1:6379" ./dfs -o allow_other /tmp/dfs


mount_null:
	make mount
	DFS_TYPE="null" ./dfs -o allow_other /tmp/dfs
	make mount

mount:
	mkdir -p /tmp/dfs 2>/dev/null

test:
	mkdir -p /tmp/dfs/tmp
	cd /tmp/dfs/tmp
	echo "hello" > hello
	echo "world" > world
	cat hello
	cat world

unmount:
	umount /tmp/dfs
