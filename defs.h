#pragma once

#include <elf.h>
#include <stdio.h>
#include <sys/stat.h>

#define XORKEY 0x19

struct elf32_t {
    struct stat st;
    char *bin;
    Elf32_Ehdr *elfhdr;
    Elf32_Phdr *phdr;
    Elf32_Shdr *shdr;
    uint32_t stubsz;
};

// dump.c
void dump_elf(const struct elf32_t *e);

// elflib.c
void memsft(char *bin, int off, size_t size);
void remove_section_name(Elf32_Ehdr *elfhdr, Elf32_Shdr *shdr);
int search_entry_phdr(const Elf32_Phdr *phdr, int phnum, uint32_t entry);
int search_entry_shdr(const Elf32_Shdr *shdr, int shnum, uint32_t entry);

// elfpack.c
void free_elf(struct elf32_t *e);
int load_elf(struct elf32_t *e, const char *fname);
int pack_elf(struct elf32_t *e);
int write_elf(const struct elf32_t *e, const char *fname);

// stub.c
void add_stub(char *addr, uint32_t size);
void make_stub(uint32_t addr, uint32_t size, uint32_t entry, uint8_t key);
uint32_t stub_size();