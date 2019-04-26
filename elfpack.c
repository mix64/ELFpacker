#include <elf.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "defs.h"

void free_elf(struct elf32_t *e)
{
    if (e->bin) {
        free(e->bin);
        e->bin = NULL;
    }
}

int load_elf(struct elf32_t *e, const char *fname)
{
    FILE *fp = 0;
    int err  = 0;

    e->stubsz = stub_size();

    if (stat(fname, &e->st)) {
        fprintf(stderr, "error: stat()\n");
        goto bad;
    }

    if (!(fp = fopen(fname, "rb"))) {
        fprintf(stderr, "error: fopen()\n");
        goto bad;
    }

    if (!(e->bin = (char *)malloc((size_t)e->st.st_size + e->stubsz))) {
        fprintf(stderr, "error: malloc()\n");
        goto bad;
    }

    if (fread(e->bin, sizeof(char), (size_t)e->st.st_size, fp) != e->st.st_size) {
        fprintf(stderr, "error: fread()\n");
        goto bad;
    }

    e->elfhdr = (Elf32_Ehdr *)e->bin;
    e->phdr   = (Elf32_Phdr *)(e->bin + e->elfhdr->e_phoff);
    e->shdr   = (Elf32_Shdr *)(e->bin + e->elfhdr->e_shoff);

    if (memcmp(e->elfhdr->e_ident, ELFMAG, SELFMAG)) {
        fprintf(stderr, "Format error: Input is not an ELF\n");
        goto bad;
    }
    if (e->elfhdr->e_ident[EI_CLASS] != ELFCLASS32) {
        fprintf(stderr, "Format error: Only support ELF32\n");
        goto bad;
    }
    if (e->elfhdr->e_shstrndx >= e->elfhdr->e_shnum) {
        fprintf(stderr, "Format error: Can't find shdr's string table\n");
        goto bad;
    }

    goto out;

bad:
    err = -1;

out:
    if (fp) {
        fclose(fp);
    }
    return err;
}

int pack_elf(struct elf32_t *e)
{
    // shift section header
    memsft(e->bin + e->elfhdr->e_shoff, (int)e->stubsz,
           (size_t)(e->elfhdr->e_shnum * e->elfhdr->e_shentsize));

    // change elfhdr info
    e->elfhdr->e_shoff += e->stubsz;
    e->shdr = (Elf32_Shdr *)(e->bin + e->elfhdr->e_shoff);

    // get shstrtab for stub
    Elf32_Shdr *stub = &e->shdr[e->elfhdr->e_shstrndx];

    // search entry header
    int pi = search_entry_phdr(e->phdr, e->elfhdr->e_phnum, e->elfhdr->e_entry);
    int si = search_entry_shdr(e->shdr, e->elfhdr->e_shnum, e->elfhdr->e_entry);
    if (pi == -1 || si == -1) {
        fprintf(stderr, "Can't find entry section/program header.\n");
        fprintf(stderr, "DEBUG: pi:%d, si:%d\n", pi, si);
        return -1;
    }

    // encode
    for (uint32_t i = 0; i < e->shdr[si].sh_size; i++) {
        e->bin[e->shdr[si].sh_offset + i] ^= XORKEY;
    }

    // change shstrtab to stub
    stub->sh_size += e->stubsz;
    stub->sh_addr      = e->phdr[pi].p_vaddr + e->phdr[pi].p_memsz;
    stub->sh_type      = SHT_PROGBITS;
    stub->sh_addralign = 1;
    stub->sh_flags     = (SHF_ALLOC | SHF_EXECINSTR);

    // make decode stub binary
    make_stub(e->shdr[si].sh_addr, e->shdr[si].sh_size, e->elfhdr->e_entry, XORKEY);

    // add decode stub
    add_stub(e->bin + stub->sh_offset, stub->sh_size);

    // change flags
    e->shdr[si].sh_flags |= SHF_WRITE;
    e->phdr[pi].p_flags |= PF_W;
    e->phdr[pi].p_filesz += e->stubsz; // TODO: if over align, need more change
    e->phdr[pi].p_memsz += e->stubsz;

    // set new entry point
    e->elfhdr->e_entry = stub->sh_addr;

    // remove section name
    remove_section_name(e->elfhdr, e->shdr);

    return 0;
}

int write_elf(const struct elf32_t *e, const char *fname)
{
    FILE *fp;

    if (!(fp = fopen(fname, "wb"))) {
        fprintf(stderr, "Can't make output file.\n");
        return -1;
    }

    fwrite(e->bin, sizeof(char), malloc_usable_size(e->bin), fp);
    fclose(fp);

    return 0;
}
