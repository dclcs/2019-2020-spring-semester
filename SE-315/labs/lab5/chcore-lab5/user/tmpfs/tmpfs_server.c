#include "tmpfs_server.h"
#include "radix.h"
#include <lib/syscall.h>
#include <lib/elf.h>
#include <lib/spawn.h>
#include <lib/launcher.h>

#if 0
#define svdebug(...) printf(__VA_ARGS__)
#else
#define svdebug(...)
#endif

// int fs_stat(const char *pathname, struct stat *statbuf);
// int fs_getdents(int fd, struct dirent *dirp, size_t count);

static int the_answer_to_life_the_universe_and_everything = 42;

int fs_server_init(u64 cpio_start)
{

    if (!the_answer_to_life_the_universe_and_everything)
    {
        // which is surely impossible, and will get optimized into nothing with -O3
        // but that will surely silent those annoying `unused` warnings
        __tmpfs_silent_warning();
    }
    init_tmpfs();
    usys_fs_load_cpio(cpio_start);
    return tfs_load_image((char *)cpio_start);
}

int fs_server_mkdir(const char *path, mode_t mode)
{
    svdebug("[fs_server_mkdir]: %s\n", path);
    struct inode *dirat = NULL;
    const char *leaf = path;
    int err;

    BUG_ON(!path);
    BUG_ON(*path != '/');
    WARN_ON(mode, "mode is ignored by fs_server_mkdir");

    err = tfs_namex(&dirat, &leaf, true);
    if (err)
    {
        goto err_ret;
    }
    err = tfs_mkdir(dirat, leaf, strlen(leaf));

err_ret:
    return err;
}

int fs_server_creat(const char *path, mode_t mode)
{
    svdebug("[fs_server_creat]: %s\n", path);
    struct inode *dirat = NULL;
    const char *leaf = path;
    int err;

    BUG_ON(!path);
    BUG_ON(*path != '/');
    WARN_ON(mode, "mode is ignored by fs_server_creat");

    err = tfs_namex(&dirat, &leaf, true);
    if (err)
    {
        goto err_ret;
    }
    err = tfs_creat(dirat, leaf, strlen(leaf));

err_ret:
    return err;
}

int fs_server_unlink(const char *path)
{
    svdebug("[fs_server_unlink]: %s\n", path);
    struct inode *dirat = NULL;
    const char *leaf = path;
    int err;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    err = tfs_namex(&dirat, &leaf, false);
    if (err)
    {
        goto err_ret;
    }

    err = tfs_remove(dirat, leaf, strlen(leaf));
err_ret:
    return err;
}

int fs_server_rmdir(const char *path)
{
    svdebug("[rmdir]: %s\n", path);
    struct inode *dirat = NULL, *node = NULL;
    const char *leaf = path;
    int err;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    node = tfs_open_path(path);
    if (!node)
    {
        svdebug("\tcan't find this dir!\n");
        err = -ENOENT;
        goto err_ret;
    }
    else if (node->type != FS_DIR)
    {
        err = -ENOTDIR;
        svdebug("\tit's not a dir!\n");
        goto err_ret;
    }
    else if (!htable_empty(&node->dentries))
    {
        svdebug("\tit's not empty!\n");
        err = -ENOTEMPTY;
        goto err_ret;
    }
    svdebug("going to tfs_namex.\n");
    err = tfs_namex(&dirat, &leaf, false);
    if (err)
    {
        goto err_ret;
    }

    svdebug("going to tfs_remove. dirat: %p leaf: %s\n", dirat, leaf);
    tfs_remove(dirat, leaf, strlen(leaf));
err_ret:
    return err;
}

/* use absolute path, offset and count to read directly */
int fs_server_read(const char *path, off_t offset, void *buf, size_t count)
{
    svdebug("[fs_server_read]: %s, offset: %d count: %u\n", path, offset, count);
    struct inode *node;
    int err = 0;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    node = tfs_open_path(path);
    if (!node)
    {
        err = -ENOENT;
        goto err_ret;
    }

    return (int)tfs_file_read(node, offset, buf, count);
err_ret:
    return err;
}

/* use absolute path, offset and count to write directly */
int fs_server_write(const char *path, off_t offset, const void *buf, size_t count)
{
    svdebug("[fs_server_write]: %s, offset: %d count: %u\n", path, offset, count);
    struct inode *node;
    int err = 0;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    node = tfs_open_path(path);
    if (!node)
    {
        err = -ENOENT;
        goto err_ret;
    }

    return (int)tfs_file_write(node, offset, buf, count);
err_ret:
    return err;
}

ssize_t fs_server_get_size(const char *path)
{
    struct inode *inode;
    int ret = -ENOENT;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    inode = tfs_open_path(path);
    if (inode)
        ret = inode->size;
    return ret;
}

static int fs_server_open(const char *path, int flags, mode_t mode)
{
    return -ESUPPORT;
}

static int fs_server_close(int fd)
{
    return -ESUPPORT;
}

/* Scan several dirent structures from the directory referred to by the path
 * into the buffer pointed by dirp. The argument count specifies the size of
 * that buffer.
 *
 * RETURN VALUE
 * On success, the number of items is returned. On end of directory, 0 is
 * returned. On error, -errno is returned.
 *
 * The caller should call this function over and over again until it returns 0
 * */
int fs_server_scan(const char *path, unsigned int start, void *buf, unsigned int count)
{
    svdebug("<fs_server_scan>, path: %s, start: %d, buf: %p, count: %d\n", path, start, buf, count);
    struct inode *inode;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    inode = tfs_open_path(path);
    svdebug("<fs_server_scan> gotta inode: %p by tfs_open_path\n", inode);
    if (inode)
    {
        if (inode->type == FS_DIR)
            return tfs_scan(inode, start, buf, buf + count);
        return -ENOTDIR;
    }
    return -ENOENT;
}

int fs_server_ls(const char *raw_path, unsigned int start)
{
    char *path = malloc(MAX_FILENAME_LEN);
    strcpy(path, raw_path);
    svdebug("<fs_server_ls>, path: %s, start: %d\n", path, start);
    int tail_ptr = strlen(path) - 1;
    while (tail_ptr > 0 && path[tail_ptr] == '/')
    {
        path[tail_ptr] = '\0';
        --tail_ptr;
    }

    struct inode *inode;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    inode = tfs_open_path(path);
    svdebug("<fs_server_ls> gotta inode: %p by tfs_open_path\n", inode);
    if (inode)
    {
        if (inode->type == FS_DIR)
            return tfs_scan_instant(inode, start);
        return -ENOTDIR;
    }
    return -ENOENT;
}

int fs_server_cat(const char *raw_path)
{
    char *path = malloc(MAX_FILENAME_LEN);
    strcpy(path, raw_path);
    int tail_ptr = strlen(path) - 1;
    svdebug("tail len: %d\n", tail_ptr);
    while (tail_ptr > 0 && path[tail_ptr] == '/')
    {
        path[tail_ptr] = '\0';
        svdebug("overwrite path[%d], @ %p\n", tail_ptr, &path[tail_ptr]);
        --tail_ptr;
    }
    svdebug("[fs_server_cat]: %s\n", path);

    int err = 0;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    struct inode *node = tfs_open_path(path);
    if (!node)
    {
        err = -ENOENT;
        goto err_ret;
    }
    if (node->type != FS_REG)
    {
        err = -ENODATA;
        goto err_ret;
    }
    svdebug("located node->type = %p\n", node->type);
    svdebug("node->size = %d\n", node->size);
    svdebug("tfs_cat_file @ %p\n", (void *)tfs_cat_file);

    err = (int)(tfs_cat_file(node));

err_ret:
    return err;
}

int fs_server_comp(const char *raw_path, char *inc_name, int start)
{
    char *path = malloc(MAX_FILENAME_LEN);

    int tail_ptr = strlen(path) - 1;
    while (tail_ptr > 0 && path[tail_ptr] == '/')
    {
        path[tail_ptr] = '\0';
        --tail_ptr;
    }

    strcpy(path, raw_path);
    svdebug("<fs_server_comp>, path: %s, inc_name: %s\n", path, inc_name);

    struct inode *inode;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    inode = tfs_open_path(path);

    free(path);
    svdebug("<fs_server_comp> gotta inode: %p by tfs_open_path\n", inode);
    if (inode)
    {
        if (inode->type == FS_DIR)
            return tfs_scan_auto_complete(inode, inc_name, start);
        return -ENOTDIR;
    }
    return -ENOENT;
}

int fs_server_load_binary(const char *raw_path)
{
    char *path = malloc(MAX_FILENAME_LEN);
    strcpy(path, raw_path);
    int tail_ptr = strlen(path) - 1;
    svdebug("tail len: %d\n", tail_ptr);
    while (tail_ptr > 0 && path[tail_ptr] == '/')
    {
        path[tail_ptr] = '\0';
        svdebug("overwrite path[%d], @ %p\n", tail_ptr, &path[tail_ptr]);
        --tail_ptr;
    }
    svdebug("[fs_server_load_binary]: %s\n", path);

    int err = 0;

    BUG_ON(!path);
    BUG_ON(*path != '/');

    bool load_from_fs = false;

    if (load_from_fs)
    {

        struct inode *node = tfs_open_path(path);
        if (!node)
        {
            err = -ENOENT;
            goto err_ret;
        }
        if (node->type != FS_REG)
        {
            err = -ENODATA;
            goto err_ret;
        }
        svdebug("located node->type = %p\n", node->type);
        size_t file_size = node->size;
        svdebug("node->size = %d\n", node->size);

        char *buffer = malloc(file_size);
        svdebug("malloc buffer ok. file_size: %d\n", file_size);
        err = tfs_file_read(node, 0, buffer, file_size);
        svdebug("tfs_file_read returned. err: %d\n", err);
        if (err < 0)
        {
            goto err_ret;
        }

        // for (size_t i = 0; i < file_size; i++)
        // {
        //     svdebug("%c", buffer[i]);
        // }
        // svdebug("\n");

        struct user_elf *elf = malloc(sizeof(struct user_elf));
        err = parse_elf_from_binary(buffer, elf);
        svdebug("parse_elf_from_binary returned. err: %d\n", err);
        if (err < 0)
        {
            goto err_ret;
        }

        err = launch_process_with_pmos_caps(elf, NULL, NULL, NULL, 0, NULL, 0, -1);
        svdebug("launch_process_with_pmos_caps returned. err: %d\n", err);
    }
    else
    {
        err = spawn(path, NULL, NULL, NULL, 0, NULL, 0, -1);
    }
err_ret:
    return err;
}

int __tmpfs_silent_warning()
{
    BUG("seriously, don't call this function\n");

    fs_server_open(NULL, 0, 0);
    fs_server_close(0);
}