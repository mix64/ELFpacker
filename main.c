#include <stdio.h>

#include "defs.h"

/* only support elf32_i386 */

#define DEBUG 1

struct elf32_t e;

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: pac <input> <output>\n");
        return -1;
    }

    if (load_elf(&e, argv[1])) {
        fprintf(stderr, "error: load_elf()\n");
        goto out;
    }

#if DEBUG
    dump_elf(&e);
#endif

    if (pack_elf(&e)) {
        fprintf(stderr, "error: pack_elf()\n");
        goto out;
    }

    if (write_elf(&e, argv[2])) {
        fprintf(stderr, "error: write_elf()\n");
        goto out;
    }

    chmod(argv[2], 0755);

out:
    free_elf(&e);
    return 0;
}