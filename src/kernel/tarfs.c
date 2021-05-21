#include "common/file.h"
#include "common/print.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "kernel/tarfs.h"

static tarfs_inode_t inodes[MAX_INODE];
static tarfs_fds_t tarfs_fds[MAX_FDS];

static size_t octal_to_decimal(char* a) {
  int len = strlen(a);
  size_t sum = 0;

  size_t pow8 = 1;
  for (int i = len - 1; i >= 0; i--) {
    sum += pow8 * ((size_t)a[i] - '0');
    pow8 *= 8;
  }
  return sum;
}

static int find_empty_fds() {
  for (int i = 0; i < MAX_FDS; i++) {
    if (tarfs_fds[i].inode == NULL)
      return i;
  }

  return -1;
}

int tarfs_init(void* opaque) {
  char* addr = (char*)opaque;

  // initialize fds
  for (int i = 0; i < MAX_FDS; i++) {
    tarfs_fds[i].inode = NULL;
    tarfs_fds[i].cursor = 0;
  }

  // initialize the inode pool
  for (int i = 0; i < MAX_INODE; i++) {
    inodes[i].data = NULL;
  }

  // parse the tarball
  int inode_curr = 0;
  tar_header_t* header = (tar_header_t*)addr;
  while (header->name[0]) {
    int size = octal_to_decimal(header->size);

    tarfs_inode_t node = {
        .name = (char*)header->name,
        .size = size,
        .attribute = (char)header->typeflag,
        .data = (uint8_t*)(header + 1),
    };

    printf("=== The %ith file ===\n", inode_curr);
    printf("name:\t%s\n"
           "size\t%i\n"
           "type\t'%c'\n",
           header->name, size, header->typeflag);

    inodes[inode_curr++] = node;

    if (size > 0) {
      header += (size / 512) + 1;
    }
    header++;
  }
  return 0;
}

int tarfs_open(const char* path, int flags) {
  (void)flags;

  tarfs_inode_t* node = NULL;
  for (int i = 0; i < MAX_FDS; i++) {
    if (!strcmp(path, inodes[i].name)) {
      node = &inodes[i];
      break;
    }
  }

  if (node == NULL)
    return -1;

  int idx = find_empty_fds();
  if (idx < 0)
    return -1;

  tarfs_fds[idx].inode = node;
  tarfs_fds[idx].cursor = 0;

  return idx;
}

ssize_t tarfs_read(int fd, void* buf, size_t count) {
  tarfs_fds_t* f = &tarfs_fds[fd];
  if (f->inode == NULL)
    return -1;

  size_t size = f->inode->size;

  if ((f->cursor + count) > size)
    count = size - f->cursor;

  memcpy(buf, f->inode->data + f->cursor, count);
  f->cursor += count;

  return count;
}

ssize_t tarfs_write(int fd, const void* buf, size_t count) {
  tarfs_fds_t* f = &tarfs_fds[fd];
  if (f->inode == NULL)
    return -1;

  size_t size = f->inode->size;

  if ((f->cursor + count) > size)
    count = size - f->cursor;

  memcpy(f->inode->data + f->cursor, buf, count);
  f->cursor += count;

  return count;
}

off_t tarfs_seek(int fd, off_t offset, int whence) {
  (void)whence;

  tarfs_fds_t* f = &tarfs_fds[fd];
  if (f->inode == NULL)
    return -1;

  off_t size = f->inode->size;

  if (offset < 0)
    return -1;
  if (offset > size)
    offset = size;

  f->cursor = offset;

  return offset;
}

int tarfs_close(int fd) {
  tarfs_fds_t* f = &tarfs_fds[fd];
  f->inode = NULL;
  f->cursor = 0;

  return 0;
}

int tarfs_remove(const char* path) {
  for (int i = 0; i < MAX_INODE; i++) {
    if (inodes[i].data == NULL)
      continue;
    if (!strcmp(path, inodes[i].name)) {
      inodes[i].data = NULL;
      return 0;
    }
  }
  return -1;
}

static bool is_in_dir(const char* dir, const tarfs_inode_t* inode) {
  char *path = inode->name;

  int dir_len = strlen(dir);
  int path_len = strlen(path);
  if (dir_len == path_len)
    return false;

  if (strncmp(dir, path, dir_len))
    return false;

  int slash_cnt = 0;
  for (int i = dir_len; i < path_len; i++) {
    if (path[i] == '/') {
      slash_cnt++;
      switch (inode->attribute) {
      case '0':
	  if (slash_cnt > 0)
	      return false;
	  break;
      case '5':
	  if (slash_cnt > 1)
	      return false;
	  break;
      default:
	  return false;
      }
    }
  }
  return true;
}

int tarfs_listdir(char* path, char *out, size_t outsize) {
  if (!strcmp(path, "."))
    path = "";

  size_t bufcnt = 0;
  int path_len = strlen(path);
  for (int i = 0; i < MAX_INODE; i++) {
    if (inodes[i].data == NULL)
      continue;

    if (is_in_dir(path, &inodes[i])) {
      bufcnt += sprintf(out + bufcnt, "%s ", inodes[i].name + path_len);
      if (bufcnt > outsize) {
	out[0] = '\0';
	return -1;
      }
    }
  }

  return bufcnt;
}
