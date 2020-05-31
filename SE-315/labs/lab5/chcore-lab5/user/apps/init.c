#include <type.h>
// deal with idiot clang-format's auto-rearranging includes
#include "init.h"
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

#define SERVER_READY_FLAG(vaddr) (*(int *)(vaddr))
#define SERVER_EXIT_FLAG(vaddr) (*(int *)((u64)vaddr + 4))

#if 0
#define shdebug(...) printf(__VA_ARGS__)
#else
#define shdebug(...)
#endif

extern ipc_struct_t *tmpfs_ipc_struct;
static ipc_struct_t ipc_struct;
static int tmpfs_scan_pmo_cap;

/* fs_server_cap in current process; can be copied to others */
int fs_server_cap;

extern char getch();

char current_path[BUFLEN] = "/";

void parse_path(const char *input, char *output)
{
    shdebug("input path: %s\n", input);
    size_t path_since = 0, path_till;
    while (input[path_since] == ' ')
    {
        shdebug("skipping space '%c'\n", input[path_since]);
        ++path_since;
    }
    if (input[path_since] && input[path_since] == '/')
    {
        // absolute
        size_t prefix_offset = 0;
        path_till = path_since;
        while (input[path_till] && input[path_till] != ' ')
        {
            shdebug("eating path char '%c'\n", input[path_till]);
            output[path_till - path_since + prefix_offset] = input[path_till];
            ++path_till;
        }
        // shdebug(result, cmdline + path_since, path_till - path_since);
        output[path_till - path_since + prefix_offset] = '\0';
    }
    else if (input[path_since])
    {
        // relative
        memcpy(output, current_path, strlen(current_path));
        size_t prefix_offset = strlen(current_path);
        path_till = path_since;
        while (input[path_till] && input[path_till] != ' ')
        {
            shdebug("eating path char '%c'\n", input[path_till]);
            output[path_till - path_since + prefix_offset] = input[path_till];
            ++path_till;
        }
        // shdebug(result, cmdline + path_since, path_till - path_since);
        output[path_till - path_since + prefix_offset] = '\0';
    }
    else
    {
        output[0] = '/';
        output[1] = '\0';
    }
    shdebug("output path: (%s)@%p\n", output, output);
}

int split_path(char *inc_path, char *buf)
{

    shdebug("going to make split_path with %s, buf: %p\n", inc_path, buf);
    memset(buf, 0, 256);
    if (strlen(inc_path) > 128)
    {
        // reject auto complete for a too-long path
        printf("shell: failed to auto-complete a oversized path\n");
        return 0;
    }

    char *temp_out = malloc(256);
    parse_path(inc_path, temp_out);

    int tail_ptr = strlen(temp_out) - 1;
    int split_ptr = tail_ptr;

    shdebug("gotta temp_out: %p init tail_ptr: %d\n", temp_out, tail_ptr);

    while (temp_out[split_ptr] != '/' && split_ptr > 0)
    {
        --split_ptr;
    }

    if (split_ptr == 0)
    {
        // auto complete at root node
        buf[0] = '/';
        strcpy(buf + 128, inc_path);
    }
    else if (split_ptr == tail_ptr)
    {
        // auto complete without any inc_path
        strcpy(buf, temp_out);
    }
    else
    {
        // normal /<path>/<inc_file>
        //              ^ split_ptr here
        temp_out[split_ptr] = 0;
        strcpy(buf, temp_out);
        strcpy(buf + 128, temp_out + split_ptr + 1);
    }

    free(temp_out);
    shdebug("split_path done. prefix path: (%s), inc_filename: (%s)\n", buf, buf + 128);
}

#define SHELL_BEEP 7
#define SHELL_TABLE 9
#define SHELL_ENTER 13
#define SHELL_ESCAPE 27
#define SHELL_UP 65
#define SHELL_DOWN 66
#define SHELL_RIGHT 67
#define SHELL_LEFT 68
#define SHELL_BACKSPACE 127

#define MAX_LEN 65

static void flush_line(size_t prefix)
{
    printf("\r");
    for (size_t i = 0; i < MAX_LEN + prefix; i++)
    {
        printf(" ");
    }
}

// read a command from stdin leading by `prompt`
// put the commond in `buf` and return `buf`
// What you typed should be displayed on the screen
char *readline(const char *prompt)
{
    char real_buf[BUFLEN] = "";
    // memset(real_buf, 0, BUFLEN);
    char *buf = real_buf + strlen(prompt);
    memcpy(real_buf, prompt, strlen(prompt));
    int i = 0, j = 0;
    signed char c = 0;
    int ret = 0;
    char complement[BUFLEN];
    int complement_time = 0;

    char last_comp[256];
    int last_request_since = 0;

    size_t prompt_len = strlen(prompt);

    if (prompt != NULL)
    {
        printf("%s", prompt);
    }

    while (1)
    {
        c = getch();
        if (c < 0)
            return NULL;

        if (c != SHELL_BACKSPACE && c != SHELL_ENTER && complement_time > MAX_LEN - 1)
        {
            // command too long!
            usys_putc(SHELL_BEEP);
            continue;
        }

        // printf("\neat chr: %d\n", (int)c);
        if (c == SHELL_BACKSPACE && complement_time > 0)
        {
            // printf("\n------\n");
            --complement_time;
            complement[complement_time] = '\0';
        }
        else if (c == SHELL_BACKSPACE)
        {
            // don't do anything if you're trying to backspace nothing
            continue;
        }
        else if (c == SHELL_ENTER || c == '\n')
        {
            break;
        }
        else if (c == SHELL_UP)
        {
            complement[complement_time++] = 'A';
        }
        else if (c == SHELL_DOWN)
        {
            complement[complement_time++] = 'B';
        }
        else if (c == SHELL_LEFT)
        {
            complement[complement_time++] = 'D';
        }
        else if (c == SHELL_RIGHT)
        {
            complement[complement_time++] = 'C';
        }
        else if (c == SHELL_ESCAPE)
        {
            complement[complement_time++] = '^';
        }
        else if (c == SHELL_TABLE)
        {
            if (last_request_since == 0)
            {
                char *tmp_inp = malloc(256);
                char *buf = malloc(256);

                memset(buf, 0, 256);
                memset(tmp_inp, 0, 256);
                memcpy(tmp_inp, complement, complement_time);
                shdebug("\ntab triggered. going to auto complete %s\n", tmp_inp);
                strcpy(last_comp, tmp_inp);
                auto_complete(tmp_inp, buf, last_request_since);
                shdebug("gotta buf: %s, len: %d\n", buf, strlen(buf));
                if (*buf)
                {
                    memset(complement, 0, 256);
                    strcpy(complement, buf);
                    complement[strlen(buf)] = '\0';
                    complement_time = strlen(buf);
                    shdebug("\ngoing to rewrite line as %s\n", buf);
                }
                else
                {
                    usys_putc(SHELL_BEEP);
                    shdebug("\nshit, nothing\n");
                }
                free(buf);
                free(tmp_inp);
                ++last_request_since;
            }
            else
            {
                char *tmp_inp = malloc(256);
                char *buf = malloc(256);

                memset(buf, 0, 256);
                memset(tmp_inp, 0, 256);
                strcpy(tmp_inp, last_comp);
                shdebug("\ntab triggered. going to auto complete %s\n", tmp_inp);

                auto_complete(tmp_inp, buf, last_request_since);
                shdebug("gotta buf: %s, len: %d\n", buf, strlen(buf));
                if (*buf)
                {
                    memset(complement, 0, 256);
                    strcpy(complement, buf);
                    complement[strlen(buf)] = '\0';
                    complement_time = strlen(buf);
                    shdebug("\ngoing to rewrite line as %s\n", buf);
                }
                else
                {
                    usys_putc(SHELL_BEEP);
                    shdebug("\nshit, nothing\n");
                }
                free(buf);
                free(tmp_inp);
                ++last_request_since;
            }
        }
        else
        {
            complement[complement_time++] = c;
        }

        if (c != SHELL_TABLE)
        {
            // clear last auto complete data
            memset(last_comp, 0, 256);
            last_request_since = 0;
        }

        memcpy(buf, complement, complement_time);
        buf[complement_time] = '\0';
        shdebug("\ncurrent real_buf: %s, buf: %s, complement: %s, len: %d\n", real_buf, buf, complement, complement_time);
        flush_line(prompt_len);
        printf("\r%s", real_buf);
    }
    printf("\n");
    return buf;
}

void auto_complete(char *inc_path, char *completion, int start)
{
    shdebug("\ncalled auto_complete: %s\n", inc_path);
    while (*inc_path == ' ')
    {
        // skip prefix spaces
        ++inc_path;
    }

    // remove suffix spaces
    int tail_node = strlen(inc_path);
    while (tail_node > 0 && inc_path[tail_node - 1] == ' ')
    {
        inc_path[tail_node - 1] = '\0';
        --tail_node;
    }

    ipc_msg_t *msg = ipc_create_msg(&ipc_struct, sizeof(struct fs_request), tmpfs_scan_pmo_cap);
    struct fs_request *fr = (struct fs_request *)malloc(sizeof(struct fs_request));

    // char *completion = malloc(256);

    split_path(inc_path, fr->path);

    fr->req = FS_REQ_COMP;

    int ret;

    fr->count = start;
    // shdebug("final path: %s\n", fr->path);
    shdebug("msg: %p fr: %p\n", msg, fr);

    ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));

    ret = ipc_call(tmpfs_ipc_struct, msg);

    struct fs_request *ret_fr = (struct fs_request *)ipc_get_msg_data(msg);
    if (ret == 0)
    {
        shdebug("prefix: %s auto completed: %s\n", ret_fr->path, ret_fr->path + 128);
        // strcpy(completion, ret_fr->path + 128);
        completion[0] = '\0';

        strcpy(completion, ret_fr->path);
        if (strcmp(completion, "/") != 0)
        {
            strcat(completion, "/");
        }
        strcat(completion, ret_fr->path + 128);

        if (strncmp(completion, current_path, strlen(current_path)) == 0)
        {
            // remove redundant prefix
            for (size_t i = 0; i < strlen(completion) - strlen(current_path) + 1; i++)
            {
                completion[i] = completion[i + strlen(current_path)];
            }
        }
    }
    else
    {
        completion[0] = '\0';
    }
}

// run `ls`, `echo`, `cat`, `cd`, `top`
// return true if `cmdline` is a builtin command
#define JUDGE_CMD(cmd, target) strncmp(cmd, target, strlen(target))
#define JUDGE_CMD_STRICT(cmd, target) strcmp(cmd, target)

int builtin_cmd(char *cmdline)
{
    shdebug("called builtin_cmd: %s\n", cmdline);
    while (*cmdline == ' ')
    {
        // skip prefix spaces
        ++cmdline;
    }

    // remove suffix spaces
    int tail_node = strlen(cmdline);
    while (tail_node > 0 && cmdline[tail_node - 1] == ' ')
    {
        cmdline[tail_node - 1] = '\0';
        --tail_node;
    }

    shdebug("parsed builtin_cmd: %s\n", cmdline);
    if (JUDGE_CMD(cmdline, "ls ") == 0)
    {
        ipc_msg_t *msg = ipc_create_msg(&ipc_struct, sizeof(struct fs_request), tmpfs_scan_pmo_cap);
        struct fs_request *fr = (struct fs_request *)malloc(sizeof(struct fs_request));

        parse_path(cmdline + 3, fr->path);

        fr->req = FS_REQ_LS;

        int ret, start = 0;

        fr->count = start;
        shdebug("final path: %s\n", fr->path);
        shdebug("msg: %p fr: %p\n", msg, fr);

        ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));

        ret = ipc_call(tmpfs_ipc_struct, msg);

        if (ret == 0)
        {
            printf("ls: get no entry in %s\n", fr->path);
        }
        else if (ret == 1)
        {
            printf("ls: get 1 entry in %s\n", fr->path);
        }
        else if (ret > 0)
        {
            printf("ls: get %d entries in %s\n", ret, fr->path);
        }
        else if (ret == -ENOENT)
        {
            printf("ls: %s doesn't exist\n", fr->path);
        }
        else if (ret == -ENOTDIR)
        {
            printf("ls: %s is not a dir\n", fr->path);
        }
        else
        {
            printf("ls: %s unknown error %d\n", fr->path, ret);
        }
        return true;
    }
    if (JUDGE_CMD_STRICT(cmdline, "ls") == 0)
    {
        ipc_msg_t *msg = ipc_create_msg(&ipc_struct, sizeof(struct fs_request), tmpfs_scan_pmo_cap);
        struct fs_request *fr = (struct fs_request *)malloc(sizeof(struct fs_request));

        strcpy(fr->path, current_path);

        fr->req = FS_REQ_LS;

        int ret, start = 0;

        fr->count = start;
        shdebug("final path: %s. buf: %p count: %d\ntmpfs_ipc_struct: %p\n", fr->path, fr->buff, fr->count, tmpfs_ipc_struct);
        shdebug("msg: %p fr: %p\n", msg, fr);

        ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));

        ret = ipc_call(tmpfs_ipc_struct, msg);

        if (ret == 0)
        {
            printf("ls: get no entry in %s\n", fr->path);
        }
        else if (ret == 1)
        {
            printf("ls: get 1 entry in %s\n", fr->path);
        }
        else if (ret > 0)
        {
            printf("ls: get %d entries in %s\n", ret, fr->path);
        }
        else if (ret == -ENOENT)
        {
            printf("ls: %s doesn't exist\n", fr->path);
        }
        else if (ret == -ENOTDIR)
        {
            printf("ls: %s is not a dir\n", fr->path);
        }
        else
        {
            printf("ls: %s unknown error %d\n", fr->path, ret);
        }
        return true;
    }
    if (JUDGE_CMD(cmdline, "echo ") == 0)
    {
        printf("%s\n", cmdline + 5);
        return true;
    }
    if (JUDGE_CMD(cmdline, "cat ") == 0)
    {
        ipc_msg_t *msg = ipc_create_msg(&ipc_struct, sizeof(struct fs_request), tmpfs_scan_pmo_cap);
        struct fs_request *fr = (struct fs_request *)malloc(sizeof(struct fs_request));

        parse_path(cmdline + 4, fr->path);
        fr->req = FS_REQ_CAT;

        int ret;

        shdebug("final path: %s. tmpfs_ipc_struct: %p\n", fr->path, tmpfs_ipc_struct);
        shdebug("msg: %p fr: %p\n", msg, fr);

        ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));

        ret = ipc_call(tmpfs_ipc_struct, msg);
        printf("\n");
        if (ret >= 0)
        {
            printf("cat: read %d bytes from %s\n", ret, fr->path);
        }
        else if (ret == -ENOENT)
        {
            printf("cat: %s doesn't exist\n", fr->path);
        }
        else if (ret == -ENODATA)
        {
            printf("cat: %s is not a regular file\n", fr->path);
        }
        else
        {
            printf("cat: %s unknown error %d\n", fr->path, ret);
        }
        return true;
    }
    if (JUDGE_CMD(cmdline, "cd ") == 0)
    {
        ipc_msg_t *msg = ipc_create_msg(&ipc_struct, sizeof(struct fs_request), tmpfs_scan_pmo_cap);
        struct fs_request *fr = (struct fs_request *)malloc(PAGE_SIZE);

        parse_path(cmdline + 3, fr->path);

        fr->req = FS_REQ_SCAN;

        int ret, start = 0;

        fr->count = start;
        shdebug("final path: %s.\ntmpfs_ipc_struct: %p\n", fr->path, tmpfs_ipc_struct);
        shdebug("msg: %p fr: %p\n", msg, fr);

        ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));

        ret = ipc_call(tmpfs_ipc_struct, msg);

        if (ret >= 0)
        {
            printf("cd: pwd changed to %s\n", fr->path);

            size_t path_len = strlen(fr->path);
            if (fr->path[path_len - 1] != '/')
            {
                // add additional `/` at the endof pwd
                fr->path[path_len] = '/';
                fr->path[path_len + 1] = '\0';
            }
            strcpy(current_path, fr->path);
        }
        else if (ret == -ENOENT)
        {
            printf("cd: %s doesn't exist\n", fr->path);
        }
        else if (ret == -ENOTDIR)
        {
            printf("cd: %s is not a dir\n", fr->path);
        }
        else
        {
            printf("cd: %s unknown error %d\n", fr->path, ret);
        }
        return true;
    }
    if (JUDGE_CMD_STRICT(cmdline, "top") == 0)
    {
        usys_top();
        return true;
    }

    if (JUDGE_CMD_STRICT(cmdline, "exit") == 0)
    {
        printf("Bye");
        usys_exit(0);

        // will never reach here~
        return true;
    }
    return false;
}

// run other command, such as execute an executable file
// return true if run sccessfully
int run_cmd(char *cmdline)
{

    shdebug("called run_cmd: %s\n", cmdline);
    while (*cmdline == ' ')
    {
        // skip prefix spaces
        ++cmdline;
    }

    // remove suffix spaces
    int tail_node = strlen(cmdline);
    while (tail_node > 0 && cmdline[tail_node - 1] == ' ')
    {
        cmdline[tail_node - 1] = '\0';
        --tail_node;
    }

    shdebug("parsed run_cmd: %s\n", cmdline);

    int tmpfs_read_pmo_cap;

    tmpfs_read_pmo_cap = usys_create_pmo(PAGE_SIZE, PMO_DATA);

    ipc_msg_t *msg = ipc_create_msg(tmpfs_ipc_struct, sizeof(struct fs_request), tmpfs_read_pmo_cap);
    struct fs_request *fr = ipc_get_msg_data(msg);
    fr->req = FS_REQ_LOAD_BINARY;
    parse_path(cmdline, fr->path);

    ipc_set_msg_data(msg, (char *)fr, 0, sizeof(struct fs_request));
    return ipc_call(tmpfs_ipc_struct, msg);
}

static int
run_cmd_from_kernel_cpio(const char *filename, int *new_thread_cap,
                         struct pmo_map_request *pmo_map_reqs,
                         int nr_pmo_map_reqs)
{
    struct user_elf user_elf;
    int ret;

    ret = readelf_from_kernel_cpio(filename, &user_elf);
    if (ret < 0)
    {
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
