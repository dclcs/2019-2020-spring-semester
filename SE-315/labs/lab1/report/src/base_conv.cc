#include <iostream>

#define MAX_INT_BUFF_SIZE 80

static void
write_string(char* str)
{
    printf("%s\n", str);
    // std::cout << std::string(str) << std::endl;
}

static void
write_num(int base_i, unsigned long n, int neg)
{
    unsigned long base = base_i;
    // neg == 0 => positive number
    // neg == 1 => negative number

    // doesn't support any base greater than hex
    if (base > 16) {
        return;
    }

    static const char hex[] = "0123456789abcdef";

    char buff[MAX_INT_BUFF_SIZE];
    char reverse_buff[MAX_INT_BUFF_SIZE];

    char* header = buff;

    if (n == 0) {
        // handle zero exclusively
        buff[0] = '0';
        buff[1] = '\0';
        write_string(buff);
        return;
    }

    if (neg) {
        *header = '-';
        ++header;
    }

    int ptr = 0;
    while (n > 0) {
        reverse_buff[ptr++] = hex[n % base];
        n /= base;
    }

    for (int i = 0; i < ptr; ++i) {
        *header = reverse_buff[ptr - i - 1];
        ++header;
    }

    *header = '\0';
    write_string(buff);
}

int main()
{
    unsigned long integer;
    int base, neg;
    std::cin >> integer >> base >> neg;
    write_num(base, integer, neg);
    return 0;
}