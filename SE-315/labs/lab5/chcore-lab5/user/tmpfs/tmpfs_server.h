#pragma once

#include "tmpfs.h"

int fs_server_init(u64 cpio_start);

int fs_server_mkdir(const char* path, mode_t mode);

int fs_server_creat(const char* path, mode_t mode);

int fs_server_unlink(const char* path);

int fs_server_rmdir(const char* path);

int fs_server_read(const char* path, off_t offset, void* buf, size_t count);

int fs_server_write(const char* path, off_t offset, const void* buf, size_t count);

ssize_t fs_server_get_size(const char* path);

int fs_server_scan(const char* path, unsigned int start, void* buf, unsigned int count);

int fs_server_ls(const char* path, unsigned int start);

int fs_server_cat(const char* raw_path);

int fs_server_comp(const char* raw_path, char* inc_name, int start);

int fs_server_load_binary(const char* raw_path);

int __tmpfs_silent_warning();