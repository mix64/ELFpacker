#include <elf.h>
#include <string.h>

#include "defs.h"

void memsft(char *bin, int off, size_t size) { memmove(bin + off, bin, size); }

void remove_section_name(Elf32_Ehdr *elfhdr, Elf32_Shdr *shdr)
{
    elfhdr->e_shstrndx = 0;
    for (int i = 0; i < elfhdr->e_shnum; i++) {
        shdr[i].sh_name = 0;
    }
};

int search_entry_phdr(const Elf32_Phdr *phdr, int phnum, uint32_t entry)
{
    for (int i = 0; i < phnum; i++) {
        if (phdr[i].p_vaddr <= entry && phdr[i].p_vaddr + phdr[i].p_memsz > entry) {
            return i;
        }
    }
    return -1;
}

int search_entry_shdr(const Elf32_Shdr *shdr, int shnum, uint32_t entry)
{
    for (int i = 0; i < shnum; i++) {
        if (shdr[i].sh_addr <= entry && shdr[i].sh_addr + shdr[i].sh_size > entry) {
            return i;
        }
    }
    return -1;
}