#pragma once

#define BUFLEN 4096
#define NAMELEN 256

void boot_fs(void);

// read a command from stdin leading by `prompt`
// put the commond in `buf` and return `buf`
char* readline(const char* prompt);

// run `ls`, `echo`, `cat`, `cd`, `top`
// return true if `cmdline` is a builtin command
int builtin_cmd(char* cmdline);

// run other command, such as execute an executable file
// return true if run sccessfully
int run_cmd(char* cmdline);

void auto_complete(char* inc_path, char* completion, int start);

void parse_path(const char* input, char* output);

int split_path(char* inc_path, char* buf);