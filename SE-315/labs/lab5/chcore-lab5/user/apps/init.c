#include <type.h>
// deal with idiot clang-format's auto-rearranging includes

#include <bug.h>
#include <defs.h>
#include <fs_defs.h>
#include <ipc.h>
#include <launcher.h>
#include <malloc.h>
#include <print.h>
#include <proc.h>
#include <string.h>
#include <syscall.h>

#define SERVER_READY_FLAG(vaddr) (*(int*)(vaddr))
#define SERVER_EXIT_FLAG(vaddr) (*(int*)((u64)vaddr + 4))

#if 1
#define shdebug(...) printf(__VA_ARGS__)
#else
#define shdebug(...)
#endif

extern ipc_struct_t* tmpfs_ipc_struct;
static ipc_struct_t ipc_struct;
static int tmpfs_scan_pmo_cap;

/* fs_server_cap in current process; can be copied to others */
int fs_server_cap;

#define BUFLEN 4096

extern char getch();

// read a command from stdin leading by `prompt`
// put the commond in `buf` and return `buf`
// What you typed should be displayed on the screen
char* readline(const char* prompt)
{
    char real_buf[BUFLEN];
    memset(real_buf, 0, BUFLEN);
    char* buf = real_buf + strlen(prompt);
    memcpy(real_buf, prompt, strlen(prompt));
    int i = 0, j = 0;
    signed char c = 0;
    int ret = 0;
    char complement[BUFLEN];
    int complement_time = 0;

    if (prompt != NULL) {
        printf("%s", prompt);
    }

    while (1) {
        c = getch();
        if (c < 0)
            return NULL;
        // printf("\n===chr id: %d\n", (int)c);
        if (c == 127 && complement_time > 0) {
            // printf("\n------\n");
            --complement_time;
            complement[complement_time] = '\0';
        } else if (c == 13) {
            memset(real_buf, 0, BUFLEN);
            complement_time = 0;
            break;
        } else {
            complement[complement_time++] = c;
        }

        memcpy(buf, complement, complement_time);
        buf[complement_time] = '\0';
        // printf("\ncurrent real_buf: %s, buf: %s, complement: %s\n", real_buf, buf, complement);
        printf("\r                                                            \r%s", real_buf);
    }

    printf("\n");
    return buf;
}

// run `ls`, `echo`, `cat`, `cd`, `top`
// return true if `cmdline` is a builtin command
#define JUDGE_CMD(cmd, target) strncmp(cmd, target, strlen(target))

int builtin_cmd(char* cmdline)
{
    shdebug("builtin_cmd with %s\n", cmdline);
    if (JUDGE_CMD(cmdline, "ls") == 0) {
        shdebug("I've gotta a LS! &ipc_struct: %p, shared_buf: %p\n", &ipc_struct, ipc_struct.shared_buf);
        ipc_msg_t* msg = ipc_create_msg(&ipc_struct, sizeof(struct fs_request), tmpfs_scan_pmo_cap);
        shdebug("msg: %p\n", msg);
        struct fs_request* fr = (struct fs_request*)malloc(PAGE_SIZE);
        shdebug("fr: %p\n", fr);
        size_t path_since = 2, path_till;
        void* buf = malloc(BUFLEN);
        memset(buf, '=', BUFLEN);
        while (cmdline[path_since] == ' ') {
            shdebug("skipping space '%c'\n", cmdline[path_since]);
            ++path_since;
        }
        if (cmdline[path_since]) {
            fr->path[0] = '/';
            path_till = path_since;
            while (cmdline[path_till] && cmdline[path_till] != ' ') {
                shdebug("eating path char '%c'\n", cmdline[path_till]);
                fr->path[path_till - path_since + 1] = cmdline[path_till];
                ++path_till;
            }
            shdebug(fr->path, cmdline + path_since, path_till - path_since);
            fr->path[path_till - path_since + 1] = '\0';
        } else {
            fr->path[0] = '/';
            fr->path[1] = '\0';
        }

        fr->req = FS_REQ_SCAN;

        int ret, start = 0;
        do {
            fr->count = start;
            shdebug("final path: %s. buf: %p count: %d\ntmpfs_ipc_struct: %p\n", fr->path, fr->buff, fr->count, tmpfs_ipc_struct);
            shdebug("msg: %p fr: %p\n", msg, fr);

            ipc_set_msg_data(msg, fr, 0, sizeof(struct fs_request));

            ret = ipc_call(tmpfs_ipc_struct, msg);

            start += ret;
        } while (ret != 0);

        return true;
    }
    if (JUDGE_CMD(cmdline, "echo ") == 0) {
        printf("%s\n", cmdline + 5);
        return true;
    }
    if (JUDGE_CMD(cmdline, "cat ") == 0) {
        return true;
    }
    if (JUDGE_CMD(cmdline, "cd ") == 0) {
        return true;
    }
    if (JUDGE_CMD(cmdline, "top") == 0) {

        return true;
    }
    return false;
}

// run other command, such as execute an executable file
// return true if run sccessfully
int run_cmd(char* cmdline)
{
    return 0;
}

static int
run_cmd_from_kernel_cpio(const char* filename, int* new_thread_cap,
    struct pmo_map_request* pmo_map_reqs,
    int nr_pmo_map_reqs)
{
    struct user_elf user_elf;
    int ret;

    ret = readelf_from_kernel_cpio(filename, &user_elf);
    if (ret < 0) {
        printf("[Shell] No such binary in kernel cpio\n");
        return ret;
    }
    return launch_process_with_pmos_caps(&user_elf, NULL, new_thread_cap,
        pmo_map_reqs, nr_pmo_map_reqs,
        NULL, 0, 0);
}

void boot_fs(void)
{
    int ret = 0;
    int info_pmo_cap;
    int tmpfs_main_thread_cap;
    struct pmo_map_request pmo_map_requests[1];

    /* create a new process */
    printf("Booting fs...\n");
    /* prepare the info_page (transfer init info) for the new process */
    info_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);
    fail_cond(info_pmo_cap < 0, "usys_create_ret ret %d\n", info_pmo_cap);

    ret = usys_map_pmo(SELF_CAP,
        info_pmo_cap, TMPFS_INFO_VADDR, VM_READ | VM_WRITE);
    fail_cond(ret < 0, "usys_map_pmo ret %d\n", ret);

    SERVER_READY_FLAG(TMPFS_INFO_VADDR) = 0;
    SERVER_EXIT_FLAG(TMPFS_INFO_VADDR) = 0;

    /* We also pass the info page to the new process  */
    pmo_map_requests[0].pmo_cap = info_pmo_cap;
    pmo_map_requests[0].addr = TMPFS_INFO_VADDR;
    pmo_map_requests[0].perm = VM_READ | VM_WRITE;
    ret = run_cmd_from_kernel_cpio("/tmpfs.srv", &tmpfs_main_thread_cap,
        pmo_map_requests, 1);
    fail_cond(ret != 0, "create_process returns %d\n", ret);

    fs_server_cap = tmpfs_main_thread_cap;

    while (SERVER_READY_FLAG(TMPFS_INFO_VADDR) != 1)
        usys_yield();

    /* register IPC client */
    tmpfs_ipc_struct = &ipc_struct;
    ret = ipc_register_client(tmpfs_main_thread_cap, tmpfs_ipc_struct);
    fail_cond(ret < 0, "ipc_register_client failed\n");

    tmpfs_scan_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);
    fail_cond(tmpfs_scan_pmo_cap < 0, "usys create_ret ret %d\n",
        tmpfs_scan_pmo_cap);

    ret = usys_map_pmo(SELF_CAP,
        tmpfs_scan_pmo_cap,
        TMPFS_SCAN_BUF_VADDR, VM_READ | VM_WRITE);
    fail_cond(ret < 0, "usys_map_pmo ret %d\n", ret);

    printf("fs is UP.\n");
}
