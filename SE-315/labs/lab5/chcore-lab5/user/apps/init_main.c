#include "init.h"

#include <launcher.h>
#include <print.h>
#include <string.h>
#include <syscall.h>

extern char current_path[BUFLEN];

// get a character from standard input
char getch()
{
    return (char)usys_getc();
}

int main(int argc, char* argv[])
{
    char* buf;
    int ret = 0;

    printf("init loads cpio image.\n");
    usys_fs_load_cpio(CPIO_BIN);

    boot_fs();

    char prefix_buffer[BUFLEN];
    char delimeter[] = "$ ";

    while (1) {
        strcpy(prefix_buffer, current_path);
        strcat(prefix_buffer, delimeter);
        buf = readline(prefix_buffer);
        // printf("obtained command %s\n", buf);
        if (buf == NULL)
            usys_exit(0);
        if (buf[0] == 0)
            continue;
        if (builtin_cmd(buf))
            continue;
        if ((ret = run_cmd(buf)) < 0) {
            printf("Cannot run %s, ERROR %d\n", buf, ret);
        }
    }

    return 0;
}
