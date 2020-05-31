#include <lib/defs.h>
#include <lib/syscall.h>
#include <lib/errno.h>
#include <lib/launcher.h>
#include <lib/ipc.h>
#include <lib/bug.h>
#include <lib/print.h>
#include <lib/string.h>
#include <lib/fs_defs.h>
#include <lib/malloc.h>

#if 0
#define lcdebug(...) printf(__VA_ARGS__)
#else
#define lcdebug(...)
#endif

#define VMR_READ (1 << 0)
#define VMR_WRITE (1 << 1)
#define VMR_EXEC (1 << 2)

#define PFLAGS2VMRFLAGS(PF)                                     \
	(((PF)&PF_X ? VMR_EXEC : 0) | ((PF)&PF_W ? VMR_WRITE : 0) | \
	 ((PF)&PF_R ? VMR_READ : 0))

#define OFFSET_MASK 0xfff

int parse_elf_from_binary(const char *binary, struct user_elf *user_elf)
{
	int ret;
	struct elf_file *elf = (struct elf_file *)malloc(sizeof(struct elf_file));
	size_t seg_sz, seg_map_sz;
	u64 p_vaddr;
	int i;
	int j;
	u64 tmp_vaddr = 0xc00000;

	elf_parse_file(binary, elf);

	/* init pmo, -1 indicates that this pmo is not used
	 *
	 * Currently, an elf file can have 2 PT_LOAD segment at most
	 */
	for (i = 0; i < 2; ++i)
		user_elf->user_elf_seg[i].elf_pmo = -1;

	for (i = 0, j = 0; i < elf->header.e_phnum; ++i)
	{
		if (elf->p_headers[i].p_type != PT_LOAD)
			continue;

		seg_sz = elf->p_headers[i].p_memsz;
		p_vaddr = elf->p_headers[i].p_vaddr;
		BUG_ON(elf->p_headers[i].p_filesz > seg_sz);
		seg_map_sz = ROUND_UP(seg_sz + p_vaddr, PAGE_SIZE) -
					 ROUND_DOWN(p_vaddr, PAGE_SIZE);

		user_elf->user_elf_seg[j].elf_pmo =
			usys_create_pmo(seg_map_sz, PMO_DATA);
		BUG_ON(user_elf->user_elf_seg[j].elf_pmo < 0);

		ret = usys_map_pmo(SELF_CAP,
						   user_elf->user_elf_seg[j].elf_pmo,
						   tmp_vaddr,
						   VM_READ | VM_WRITE);
		BUG_ON(ret < 0);

		memset((void *)tmp_vaddr, 0, seg_map_sz);
		/*
		 * OFFSET_MASK is for calculating the final offset for loading
		 * different segments from ELF.
		 * ELF segment can specify not aligned address.
		 *
		 */
		memcpy((void *)tmp_vaddr +
				   (elf->p_headers[i].p_vaddr & OFFSET_MASK),
			   (void *)(binary +
						elf->p_headers[i].p_offset),
			   elf->p_headers[i].p_filesz);

		user_elf->user_elf_seg[j].seg_sz = seg_sz;
		user_elf->user_elf_seg[j].p_vaddr = p_vaddr;
		user_elf->user_elf_seg[j].flags =
			PFLAGS2VMRFLAGS(elf->p_headers[i].p_flags);
		usys_unmap_pmo(SELF_CAP,
					   user_elf->user_elf_seg[j].elf_pmo,
					   tmp_vaddr);

		j++;
	}

	user_elf->elf_meta.phdr_addr = elf->p_headers[0].p_vaddr +
								   elf->header.e_phoff;
	user_elf->elf_meta.phentsize = elf->header.e_phentsize;
	user_elf->elf_meta.phnum = elf->header.e_phnum;
	user_elf->elf_meta.flags = elf->header.e_flags;
	user_elf->elf_meta.entry = elf->header.e_entry;

	return 0;
}

void *single_file_handler(const void *start, size_t size, void *data)
{
	struct user_elf *user_elf = data;
	void *ret;

	ret = (void *)(s64)parse_elf_from_binary(start, user_elf);
	return ret;
}

int readelf_from_kernel_cpio(const char *filename, struct user_elf *user_elf)
{
	int ret;

	strcpy(user_elf->path, filename);
	ret = (int)(s64)cpio_extract_single((void *)CPIO_BIN, filename,
										single_file_handler, user_elf);

	return ret;
}

ipc_struct_t *tmpfs_ipc_struct;

// read binary file according to pathname stored in `pathbuf` from FS server
// parse elf from this binary and store it in `user_elf`
int readelf_from_fs(const char *pathbuf, struct user_elf *user_elf)
{
	BUG("don't call readelf_from_fs. not implemented this way.");
	return -1;
	// lcdebug("entered readelf_from_fs\n");
	// int ret, size;
	// int tmpfs_read_pmo_cap, tmpfs_chunk_pmo_cap;

	// tmpfs_read_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);

	// ipc_msg_t *msg = ipc_create_msg(tmpfs_ipc_struct, sizeof(struct fs_request), tmpfs_read_pmo_cap);
	// struct fs_request *fr = ipc_get_msg_data(msg);
	// fr->req = FS_REQ_GET_SIZE;
	// strcpy(fr->path, pathbuf);

	// ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));
	// ret = ipc_call(tmpfs_ipc_struct, msg);
	// size = ret;

	// lcdebug("obtained file size %d\n", size);

	// if (size <= 0)
	// {
	// 	return -ENODATA;
	// }

	// tmpfs_chunk_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);
	// printf("shared buf len: %d\n", tmpfs_ipc_struct->shared_buf_len);
	// msg = ipc_create_msg(tmpfs_ipc_struct, sizeof(struct fs_request) + LAUNCHER_CHUNK_SIZE, tmpfs_chunk_pmo_cap);
	// lcdebug("msg created ok\n");
	// fr = ipc_get_msg_data(msg);
	// lcdebug("located fr: %p\n", fr);
	// strcpy(fr->path, pathbuf);
	// fr->req = FS_REQ_LOAD_BINARY;
	// fr->buff = sizeof(struct fs_request);

	// char *real_content = malloc(size);
	// lcdebug("real_content malloc ok\n");

	// int read_bytes = 0, counter = 0;
	// while (read_bytes < size)
	// {
	// 	int chunk_size = LAUNCHER_CHUNK_SIZE;
	// 	if (size - read_bytes < LAUNCHER_CHUNK_SIZE)
	// 	{
	// 		chunk_size = size - read_bytes;
	// 	}
	// 	fr->offset = read_bytes;
	// 	fr->count = chunk_size;

	// 	ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request) + LAUNCHER_CHUNK_SIZE);
	// 	ret = ipc_call(tmpfs_ipc_struct, msg);
	// 	char *buffer = ipc_get_msg_data(msg) + sizeof(struct fs_request);

	// 	// lcdebug("\n\nactual read bytes: %d\nalready read bytes: %d\nloop_time: %d\n\n", ret, read_bytes, counter);
	// 	// for (size_t i = 0; i < ret; i++)
	// 	// {
	// 	// 	lcdebug("%c", buffer[i]);
	// 	// }
	// 	// lcdebug("\n");
	// 	printf("going to memcpy %p to %p, %d bytes\n", buffer, real_content + read_bytes, chunk_size);
	// 	memcpy(real_content + read_bytes, buffer, chunk_size);

	// 	read_bytes += chunk_size;
	// 	++counter;
	// }

	// // for (size_t i = 0; i < size; i++)
	// // {
	// // 	lcdebug("%c", real_content[i]);
	// // }

	// lcdebug("\n");

	// ret = parse_elf_from_binary(real_content, user_elf);

	// lcdebug("gotta elf: path %s\n", user_elf->path);
	// for (size_t i = 0; i < 2; i++)
	// {
	// 	struct user_elf_seg *seg = &user_elf->user_elf_seg[i];
	// 	lcdebug("segment #%d: size: %u addr: %p, elf_pmo: %d\n", i, seg->seg_sz, seg->p_vaddr, seg->elf_pmo);
	// }
	// free(real_content);
	// return ret;
}
