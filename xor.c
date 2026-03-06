#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

static const uint32_t table_key = 0x6d53d2c2;

void xor_buf(uint8_t *buf, size_t len)
{
    uint8_t key = (table_key & 0xff) ^
                  ((table_key >> 8) & 0xff) ^
                  ((table_key >> 16) & 0xff) ^
                  ((table_key >> 24) & 0xff);

    for (size_t i = 0; i < len; i++)
        buf[i] ^= key;
}

void print_hex(uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        printf("\\x%02X", buf[i]);
    printf("\n");
}

void usage(char *prog)
{
    printf("Usage: %s <type> <value>\n", prog);
    printf("Types:\n");
    printf("  string <text>\n");
    printf("  ip <ipv4>\n");
    printf("  uint32 <num>\n");
    printf("  uint16 <num>\n");
    printf("  uint8  <num>\n");
    printf("  bool   <true|false>\n");
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        usage(argv[0]);
        return 1;
    }

    uint8_t buf[256];
    size_t len = 0;

    if (strcmp(argv[1], "string") == 0)
    {
        len = strlen(argv[2]) + 1;
        memcpy(buf, argv[2], len);
    }

    else if (strcmp(argv[1], "ip") == 0)
    {
        struct in_addr addr;

        if (inet_pton(AF_INET, argv[2], &addr) != 1)
        {
            printf("Invalid IP\n");
            return 1;
        }

        memcpy(buf, &addr.s_addr, sizeof(uint32_t));
        len = sizeof(uint32_t);
    }

    else if (strcmp(argv[1], "uint32") == 0)
    {
        uint32_t v = htonl(strtoul(argv[2], NULL, 10));
        memcpy(buf, &v, sizeof(v));
        len = sizeof(v);
    }

    else if (strcmp(argv[1], "uint16") == 0)
    {
        uint16_t v = htons(strtoul(argv[2], NULL, 10));
        memcpy(buf, &v, sizeof(v));
        len = sizeof(v);
    }

    else if (strcmp(argv[1], "uint8") == 0)
    {
        uint8_t v = strtoul(argv[2], NULL, 10);
        buf[0] = v;
        len = 1;
    }

    else if (strcmp(argv[1], "bool") == 0)
    {
        if (strcmp(argv[2], "true") == 0)
            buf[0] = 1;
        else if (strcmp(argv[2], "false") == 0)
            buf[0] = 0;
        else
        {
            printf("Bool must be true or false\n");
            return 1;
        }

        len = 1;
    }

    else
    {
        printf("Unknown type\n");
        return 1;
    }

    printf("XOR'ing %zu bytes...\n", len);

    xor_buf(buf, len);
    print_hex(buf, len);

    return 0;
}
