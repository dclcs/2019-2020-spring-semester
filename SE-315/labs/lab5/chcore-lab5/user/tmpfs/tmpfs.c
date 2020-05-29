#include "tmpfs.h"

#include <cpio.h>
#include <defs.h>
#include <launcher.h>
#include <string.h>
#include <syscall.h>

#if 0
#define fsdebug(...) printf(__VA_ARGS__)
#else
#define fsdebug(...)
#endif

static struct inode* tmpfs_root;

/*
 * Helper functions to calucate hash value of string
 */
static inline u64 hash_chars(const char* str, ssize_t len)
{
    u64 seed = 131; /* 31 131 1313 13131 131313 etc.. */
    u64 hash = 0;
    int i;

    if (len < 0) {
        while (*str) {
            hash = (hash * seed) + *str;
            str++;
        }
    } else {
        for (i = 0; i < len; ++i)
            hash = (hash * seed) + str[i];
    }

    return hash;
}

/* BKDR hash */
static inline u64 hash_string(struct string* s)
{
    return (s->hash = hash_chars(s->str, s->len));
}

static inline int init_string(struct string* s, const char* name, size_t len)
{
    int i;

    s->str = malloc(len + 1);
    if (!s->str)
        return -ENOMEM;
    s->len = len;

    for (i = 0; i < len; ++i)
        s->str[i] = name[i];
    s->str[len] = '\0';

    hash_string(s);
    return 0;
}

/*
 *  Helper functions to create instances of key structures
 */
static inline struct inode* new_inode(void)
{
    struct inode* inode = malloc(sizeof(*inode));

    if (!inode)
        return ERR_PTR(-ENOMEM);

    inode->type = 0;
    inode->size = 0;

    return inode;
}

static struct inode* new_dir(void)
{
    struct inode* inode;

    inode = new_inode();
    if (IS_ERR(inode))
        return inode;
    inode->type = FS_DIR;
    init_htable(&inode->dentries, 1024);

    return inode;
}

static struct inode* new_reg(void)
{
    struct inode* inode;

    inode = new_inode();
    if (IS_ERR(inode))
        return inode;
    inode->type = FS_REG;
    init_radix_w_deleter(&inode->data, free);

    return inode;
}

static struct dentry*
new_dent(struct inode* inode, const char* name, size_t len)
{
    struct dentry* dent;
    int err;

    dent = malloc(sizeof(*dent));
    if (!dent)
        return ERR_PTR(-ENOMEM);
    err = init_string(&dent->name, name, len);
    if (err) {
        free(dent);
        return ERR_PTR(err);
    }
    dent->inode = inode;

    return dent;
}

// look up a file called `name` under the inode `dir`
// and return the dentry of this file
static struct dentry* tfs_lookup(struct inode* dir, const char* name, size_t len)
{
    // fsdebug("<tfs_lookup> parent dir: %p name: %s len: %d\n", dir, name, len);
    u64 hash = hash_chars(name, len);
    struct dentry* dent;
    struct hlist_head* head;

    head = htable_get_bucket(&dir->dentries, (u32)hash);
    // fsdebug("head = %p, when lookup, hashed chars = %u\n", head, (u32)hash);
    for_each_in_hlist(dent, node, head)
    {
        // fsdebug("\tjudging %s and %s. dent->name.len = %d, len = %d\n", dent->name.str, name, dent->name.len, len);
        if (dent->name.len == len && 0 == strcmp(dent->name.str, name))
            return dent;
    }
    // fsdebug("tfs_lookup quit without anything\n");
    return NULL;
}

// this function create a file (directory if `mkdir` == true, otherwise regular
// file) and its size is `len`. You should create an inode and corresponding
// dentry, then add dentry to `dir`'s htable by `htable_add`.
// Assume that no separator ('/') in `name`.
static int tfs_mknod(struct inode* dir, const char* name, size_t len, int mkdir)
{

    fsdebug("<tfs_mknod> parent dir: %p name: %s len: %d mkdir: %d\n", dir, name, len, mkdir);

    BUG_ON(!name);

    if (tfs_lookup(dir, name, len)) {
        // reject duplicate mknod
        return -EEXIST;
    }

    if (len == 0) {
        WARN("mknod with len of 0");
        return -ENOENT;
    }

    struct inode* inode;

    if (mkdir) {
        inode = new_dir();
    } else {
        inode = new_reg();
    }
    struct dentry* entry = new_dent(inode, name, len);
    init_hlist_node(&entry->node);
    fsdebug("gotta dentry: %p\n", entry);
    fsdebug("when create, hashed chars = %u\n", (u32)hash_chars(name, len));
    htable_add(&dir->dentries, (u32)hash_chars(name, len), &entry->node);
    fsdebug("<tfs_mknod> created node. node: %p next: %p pprev: %p\n", entry->node, entry->node.next, entry->node.pprev);
    return 0;
}

int tfs_mkdir(struct inode* dir, const char* name, size_t len)
{
    return tfs_mknod(dir, name, len, 1 /* make directiory */);
}

int tfs_creat(struct inode* dir, const char* name, size_t len)
{
    return tfs_mknod(dir, name, len, 0 /* make regular file */);
}

// Walk the file system structure to locate a file with the pathname stored in `*name`
// and saves parent dir to `*dirat` and the filename to `*name`.
// If `mkdir_p` is true, you need to create intermediate directories when it missing.
// If the pathname `*name` starts with '/', then lookup starts from `tmpfs_root`,
// else from `*dirat`.
// Note that when `*name` ends with '/', the inode of last component will be
// saved in `*dirat` regardless of its type (e.g., even when it's FS_REG) and
// `*name` will point to '\0'
int tfs_namex(struct inode** dirat, const char** name, int mkdir_p)
{
    BUG_ON(dirat == NULL);
    BUG_ON(name == NULL);
    BUG_ON(*name == NULL);
    fsdebug("<tfs_namex> dirat: %p name: %p *name: %s mkdir_p: %d\n", *dirat, name, *name, mkdir_p);

    char* buff;
    int i;
    int err;

    if (**name == '/') {
        *dirat = tmpfs_root;
        // make sure `name` starts with actual name
        while (**name && **name == '/')
            ++(*name);
    } else {
        BUG_ON(*dirat == NULL);
        BUG_ON((*dirat)->type != FS_DIR);
    }

    // make sure a child name exists
    if (!**name)
        return -EINVAL;

    struct string temp_string;

    while (true) {

        size_t name_len;

        bool is_trailing = false;

        fsdebug("\n\n");

        buff = malloc(MAX_FILENAME_LEN + 1);

        while (true) {
            name_len = 0;
            while (**name && **name != '/') {
                BUG_ON(name_len > MAX_FILENAME_LEN);
                fsdebug("%u'%c' ", name_len, **name);
                buff[name_len] = **name;
                ++name_len;
                ++*name;
            }

            if (**name == '/') {
                // skip separator'/'
                ++*name;
                is_trailing = true;
            } else {
                is_trailing = false;
            }

            // filter self-pointing `.`
            if (name_len == 1 && buff[0] == '.') {
                continue;
            } else {
                break;
            }

            if (name_len != 0) {
                buff[name_len] = '\0';
            } else {
                return 0;
            }
        }

        fsdebug("\n\n");
        fsdebug("going to init_string with %s, len = %u\n", buff, name_len);

        temp_string.str = buff;
        temp_string.len = name_len;

        hash_string(&temp_string);

        bool is_tail = !**name;

        fsdebug("parsed file name: (%s) is tail: %d, is trailing: %d\n", temp_string.str, is_tail, is_trailing);

        struct dentry* tail = tfs_lookup(*dirat, temp_string.str, temp_string.len);

        if (is_tail) {
            *name = temp_string.str;
            if (tail) {
                if (is_trailing) {
                    // this is the last one & it ends with /
                    *dirat = tail->inode;
                    *name = NULL;
                    fsdebug("this is the last one & it ends with /. set *dirat as %p\n", tail->inode);
                } else if (tail->inode->type == FS_DIR) {
                    // this is the last folder though it doesn't ends with /
                    fsdebug("this is the last folder though it doesn't ends with /. set *dirat as %p\n", tail->inode);
                } else {
                    // this is the last file and it doesn't ends with /
                    // so we don't have to do anything special
                    fsdebug("this is the last file and it doesn't ends with /.\n");
                }
            } else {
                fsdebug("i wish i could do something, but didn't get anything from tfs_lookup\n");
            }
            return 0;
        }

        if ((*dirat)->type == FS_DIR) {
            fsdebug("traversing a intermediate folder\n");
            struct dentry* temp = tfs_lookup(*dirat, temp_string.str, temp_string.len);
            fsdebug("lookup it\n");
            if (!temp) {

                if (mkdir_p) {
                    err = tfs_mkdir(*dirat, temp_string.str, temp_string.len);
                    if (err) {
                        return err;
                    }

                    temp = tfs_lookup(*dirat, temp_string.str, temp_string.len);
                } else {
                    fsdebug("expected a intermediate dir (%s), but no found and mkdir_p isn't true\n", temp_string.str);
                    return -ENOENT;
                }
            }

            *dirat = temp->inode;

            if ((*dirat)->type != FS_DIR) {
                fsdebug("expected a intermediate dir, but it (%s) isn't\n", temp_string.str);
                return -ENOTDIR;
            }
        } else if ((*dirat)->type == FS_REG) {
            fsdebug("found a FS_REG (%s) in middle\n", temp_string.str);
            return -ENOTDIR;
        } else {
            return -EINVAL;
        }
    }

    /* we will never reach here? */
    /* maybe. */
    return 0;
}

int tfs_remove(struct inode* dir, const char* name, size_t len)
{
    fsdebug("<tfs_remove> parent inode: %p *name: %s len: %u\n", dir, name, len);
    u64 hash = hash_chars(name, len);
    struct dentry *dent, *target = NULL;
    struct hlist_head* head;

    BUG_ON(!name);

    if (len == 0) {
        WARN("remove with len of 0");
        return -ENOENT;
    }

    head = htable_get_bucket(&dir->dentries, (u32)hash);

    for_each_in_hlist(dent, node, head)
    {
        if (dent->name.len == len && 0 == strcmp(dent->name.str, name)) {
            target = dent;
            break;
        }
    }

    if (!target)
        return -ENOENT;

    BUG_ON(!target->inode);

    // FIXME(TCZ): remove only when file is closed by all processes
    if (target->inode->type == FS_REG) {
        // free radix tree
        radix_free(&target->inode->data);
        // free inode
        free(target->inode);
        // remove dentry from parent
        htable_del(&target->node);
        // free dentry
        free(target);
    } else if (target->inode->type == FS_DIR) {
        if (!htable_empty(&target->inode->dentries))
            return -ENOTEMPTY;

        // free htable
        htable_free(&target->inode->dentries);
        // free inode
        free(target->inode);
        // remove dentry from parent
        htable_del(&target->node);
        // free dentry
        free(target);
    } else {
        BUG("inode type that shall not exist");
    }

    return 0;
}

int init_tmpfs(void)
{
    tmpfs_root = new_dir();
    return 0;
}

// write memory into `inode` at `offset` from `buf` for length is `size`
// it may resize the file
// `radix_get`, `radix_add` are used in this function
// You can use memory functions defined in libc
ssize_t tfs_file_write(struct inode* inode, off_t offset, const char* data,
    size_t size)
{
    BUG_ON(inode->type != FS_REG);
    BUG_ON(offset > inode->size);
    fsdebug("<tfs_file_write> inode: %p offset: %d data: %p size: %d\n", inode, offset, data, size);

    u64 page_no, page_off;
    size_t to_write_bytes, written_bytes;
    void* page;

    page_no = offset / PAGE_SIZE;
    page_off = offset % PAGE_SIZE;

    to_write_bytes = size;
    written_bytes = 0;

    char buf[PAGE_SIZE];

    while (written_bytes < to_write_bytes) {
        page = radix_get(&inode->data, page_no);
        if (!page) {
            page = buf;
        }
        size_t current_page_bytes = PAGE_SIZE - page_off;
        size_t remain_bytes = to_write_bytes - written_bytes;

        if (current_page_bytes < remain_bytes) {
            memcpy(page + page_off, data + written_bytes, current_page_bytes);

            written_bytes += current_page_bytes;

            ++page_no;
            page_off = 0;
        } else {
            memcpy(page + page_off, data + written_bytes, remain_bytes);
            written_bytes += remain_bytes;
        }

        page = radix_add(&inode->data, page_no, page);
    }

    if (offset + written_bytes > inode->size) {
        inode->size = offset + written_bytes;
    }

    return written_bytes;
}

// read memory from `inode` at `offset` in to `buf` for length is `size`, do not
// exceed the file size
// `radix_get` is used in this function
// You can use memory functions defined in libc
ssize_t tfs_file_read(struct inode* inode, off_t offset, char* buf, size_t size)
{
    BUG_ON(inode->type != FS_REG);
    BUG_ON(offset > inode->size);
    fsdebug("<tfs_file_read> inode: %p offset: %d buf: %p size: %d\n", inode, offset, buf, size);

    u64 page_no, page_off;
    size_t to_read_bytes, read_bytes;

    page_no = offset / PAGE_SIZE;
    page_off = offset % PAGE_SIZE;

    void* page;

    char buff[PAGE_SIZE];
    memset(buff, 0, PAGE_SIZE);

    to_read_bytes = size;
    if (to_read_bytes + offset > inode->size) {
        to_read_bytes = inode->size - offset;
    }

    // to_read_bytes is confirmed without overflow
    read_bytes = 0;

    while (read_bytes < to_read_bytes) {
        page = radix_get(&inode->data, page_no);
        if (!page) {
            // provide paddings for unmapped pages
            page = (void*)buff;
        }
        size_t current_page_bytes = PAGE_SIZE - page_off;
        size_t remain_bytes = to_read_bytes - read_bytes;

        if (current_page_bytes < remain_bytes) {
            memcpy(buf + read_bytes, page + page_off, current_page_bytes);
            read_bytes += current_page_bytes;

            ++page_no;
            page_off = 0;
        } else {
            memcpy(buf + read_bytes, page + page_off, remain_bytes);
            read_bytes += remain_bytes;
        }
    }

    return read_bytes;
}

// load the cpio archive into tmpfs with the begin address as `start` in memory
// You need to create directories and files if necessary. You also need to write
// the data into the tmpfs.
int tfs_load_image(const char* start)
{
    fsdebug("<tfs_load_image> entered\n");
    struct cpio_file* f;
    struct inode* dirat;
    struct dentry* dent;
    const char* leaf;
    size_t len;
    int err;
    ssize_t write_count;

    BUG_ON(start == NULL);

    cpio_init_g_files();
    cpio_extract(start, "/");

    fsdebug("going to get initial dirat\n");
    char root_path[] = "/";
    dirat = tmpfs_root;

    for (f = g_files.head.next; f; f = f->next) {
        fsdebug("going to handle entry %s, data: %p size: %d\n", f->name, f->data, f->header.c_filesize);
        if (f->name[0] == '.' && f->name[1] == '\0') {
            // do not create `.`
            continue;
        }

        struct inode *parent = tmpfs_root, *created = tmpfs_root;
        char* name = f->name;

        err = tfs_namex(&parent, &name, true);
        fsdebug("parent folder found. parent: %p ret: %d\n", parent, err);
        if (err) {
            return err;
        }

        size_t name_len = strlen(name);
        fsdebug("parsed to create name: %s  len: %u\n", name, name_len);

        if (f->header.c_filesize == 0) {
            err = tfs_mkdir(parent, name, name_len);
            fsdebug("new folder created. parsed file: %s ret: %d\n", name, err);
            if (err) {
                return err;
            }
            fsdebug("\n");
        } else {
            err = tfs_creat(parent, name, name_len);
            fsdebug("new file created. parsed file: %s ret: %d\n", name, err);
            if (err) {
                return err;
            }

            name = f->name;

            err = tfs_namex(&created, &name, false);
            fsdebug("new file's parent located. parsed name: %s ret: %d parent inode: %p\n", name, err, created);
            if (err) {
                return err;
            }

            struct dentry* result = tfs_lookup(created, name, name_len);
            fsdebug("new file located. result: %p result->inode: %p\n", result, result->inode);
            size_t written_bytes = tfs_file_write(result->inode, 0, f->data, f->header.c_filesize);
            fsdebug("written %u bytes\n", written_bytes);

            fsdebug("\n");
        }
    }

    return 0;
}

static int dirent_filler(void** dirpp, void* end, char* name, off_t off,
    unsigned char type, ino_t ino)
{
    fsdebug("<dirent_filler> dirpp: %p end: %p name: %s\n", dirpp, end, name);
    struct dirent* dirp = *(struct dirent**)dirpp;
    void* p = dirp;
    unsigned short len = strlen(name) + 1 + sizeof(dirp->d_ino) + sizeof(dirp->d_off) + sizeof(dirp->d_reclen) + sizeof(dirp->d_type);
    p += len;
    if (p > end)
        return -EAGAIN;
    dirp->d_ino = ino;
    dirp->d_off = off;
    dirp->d_reclen = len;
    dirp->d_type = type;

    strcpy(dirp->d_name, name);
    *dirpp = p;

    fsdebug("<dirent_filler> written bytes in %p, and completed successfully with %d\n", dirp->d_name, len);
    return len;
}

int tfs_scan(struct inode* dir, unsigned int start, void* buf, void* end)
{
    fsdebug("<tfs_scan> dir: %p start: %d buf: %p end: %p\n", dir, start, buf, end);
    s64 cnt = 0;
    int b;
    int ret;
    ino_t ino;
    void* p = buf;
    unsigned char type;
    struct dentry* iter;

    for_each_in_htable(iter, b, node, &dir->dentries)
    {
        fsdebug("traversing htable. current list head: %p iter: %p node: %p, iter->node.next: %p pprev: %p, *pprev: %p\n", &dir->dentries.buckets[b], iter, iter->node, iter->node.next, iter->node.pprev, *(iter->node.pprev));

        if (cnt >= start) {
            type = iter->inode->type;
            ino = iter->inode->size;
            ret = dirent_filler(&p, end, iter->name.str,
                cnt, type, ino);
            if (ret <= 0) {
                return cnt - start;
            }
        }
        cnt++;
    }
    return cnt - start;
}

int tfs_scan_instant(struct inode* dir, unsigned int start)
{
    fsdebug("<tfs_scan_instant> dir: %p start: %d\n", dir, start);
    s64 cnt = 0;
    int b;

    struct dentry* iter;

    for_each_in_htable(iter, b, node, &dir->dentries)
    {
        if (cnt >= start) {

            printf("%s\n", iter->name.str);
        }
        cnt++;
    }
    return cnt - start;
}

/* path[0] must be '/' */
struct inode* tfs_open_path(const char* path)
{
    fsdebug("<fs_open_path> called with %s.\n", path);
    struct inode* dirat = NULL;
    const char* leaf = path;
    struct dentry* dent;
    int err;

    if (*path == '/' && !*(path + 1))
        return tmpfs_root;

    err = tfs_namex(&dirat, &leaf, 0);
    fsdebug("<fs_open_path> located its parent %p and name %s.\n", dirat, leaf);
    if (err)
        return NULL;

    dent = tfs_lookup(dirat, leaf, strlen(leaf));
    fsdebug("<fs_open_path> located its entry %p.\n", dent);
    return dent ? dent->inode : NULL;
}
