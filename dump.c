#include <elf.h>
#include <stdio.h>

#include "defs.h"

void dump_elfhdr(const Elf32_Ehdr *elfhdr);
void dump_phdr(const Elf32_Phdr *phdr);
void dump_shdr(const Elf32_Shdr *shdr, const char *strtab, uint32_t tabsize);

void dump_elf(const struct elf32_t *e)
{
    printf("----------------------------------------\n");
    dump_elfhdr(e->elfhdr);
    printf("----------------------------------------\n");
    for (int i = 0; i < e->elfhdr->e_phnum; i++) {
        dump_phdr(e->phdr + i);
    }
    printf("----------------------------------------\n");
    for (int i = 0; i < e->elfhdr->e_shnum; i++) {
        dump_shdr(e->shdr + i, &e->bin[e->shdr[e->elfhdr->e_shstrndx].sh_offset],
                  e->shdr[e->elfhdr->e_shstrndx].sh_size);
    }
    printf("----------------------------------------\n");
}

void dump_elfhdr(const Elf32_Ehdr *elfhdr)
{
    // dump e_ident
    printf("e_ident    :");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", elfhdr->e_ident[i]);
    }
    putchar('\n');

    // class
    switch (elfhdr->e_ident[EI_CLASS]) {
    case ELFCLASS32:
        printf("ELF32 - ");
        break;
    case ELFCLASS64:
        printf("ELF64 - ");
        break;
    default:
        printf("Invalid class - ");
        break;
    }

    // data coding
    switch (elfhdr->e_ident[EI_DATA]) {
    case ELFDATA2LSB:
        printf("2's complement - little endian - ");
        break;
    case ELFDATA2MSB:
        printf("2's complement - big endian - ");
        break;
    default:
        printf("Invalid data format - ");
        break;
    }

    // OS/ABI
    if (elfhdr->e_ident[EI_OSABI] == ELFOSABI_SYSV) {
        printf("UNIX System V ABI.");
    }
    else {
        printf("Other OS/ABI. (see elf.h)");
    }
    putchar('\n');

    printf("e_type     :0x%x ", elfhdr->e_type);
    switch (elfhdr->e_type) {
    case ET_NONE:
        printf("(ET_NONE) No file type.");
        break;
    case ET_REL:
        printf("(ET_REL) Relocatable file.");
        break;
    case ET_EXEC:
        printf("(ET_EXEC) Executable file.");
        break;
    case ET_CORE:
        printf("(ET_CORE) Core file.");
        break;
    case ET_LOPROC:
        printf("(ET_LOPROC) Processor-specific file.");
        break;
    case ET_HIPROC:
        printf("(ET_HIPROC) Processor-specific file.");
        break;
    default:
        printf("(---------) Unknown type.");
        break;
    }
    putchar('\n');

    printf("e_machine  :0x%x (See elh.h)\n", elfhdr->e_machine);
    printf("e_version  :0x%x\n", elfhdr->e_version);
    printf("e_entry    :0x%x\n", elfhdr->e_entry);
    printf("e_phoff    :0x%x\n", elfhdr->e_phoff);
    printf("e_shoff    :0x%x\n", elfhdr->e_shoff);
    printf("e_flags    :0x%x\n", elfhdr->e_flags);
    printf("e_ehsize   :0x%x\n", elfhdr->e_ehsize);
    printf("e_phentsize:0x%x\n", elfhdr->e_phentsize);
    printf("e_phnum    :0x%x\n", elfhdr->e_phnum);
    printf("e_shentsize:0x%x\n", elfhdr->e_shentsize);
    printf("e_shnum    :0x%x\n", elfhdr->e_shnum);
    printf("e_shstrndx :0x%x\n", elfhdr->e_shstrndx);
    putchar('\n');
}

void dump_phdr(const Elf32_Phdr *phdr)
{
    printf("p_type  :");
    switch (phdr->p_type) {
    case PT_NULL:
        printf("(PT_NULL) Program header table entry unused.");
        break;
    case PT_LOAD:
        printf("(PT_LOAD) Loadable program segment.");
        break;
    case PT_DYNAMIC:
        printf("(PT_DYNAMIC) Dynamic linking information.");
        break;
    case PT_INTERP:
        printf("(PT_INTERP) Program interpreter.");
        break;
    case PT_NOTE:
        printf("(PT_NOTE) Auxiliary information.");
        break;
    case PT_SHLIB:
        printf("(PT_SHLIB) Reserved.");
        break;
    case PT_PHDR:
        printf("(PT_PHDR) Entry for header table itself.");
        break;
    case PT_TLS:
        printf("(PT_TLS) Thread-local storage segment.");
        break;
    default:
        printf("(------) Unknown segment type.");
        break;
    }
    putchar('\n');

    printf("p_offset:0x%x\n", phdr->p_offset);
    printf("p_vaddr :0x%x\n", phdr->p_vaddr);
    printf("p_paddr :0x%x\n", phdr->p_paddr);
    printf("p_filesz:0x%x\n", phdr->p_filesz);
    printf("p_memsz :0x%x\n", phdr->p_memsz);

    printf("p_flags :");
    (phdr->p_flags & PF_R) ? printf("R") : printf("-");
    (phdr->p_flags & PF_W) ? printf("W") : printf("-");
    (phdr->p_flags & PF_X) ? printf("X") : printf("-");
    putchar('\n');

    printf("p_align :0x%x\n", phdr->p_align);
    putchar('\n');
}

void dump_shdr(const Elf32_Shdr *shdr, const char *strtab, uint32_t tabsize)
{
    printf("sh_name     :");
    if (shdr->sh_name < tabsize && strtab[shdr->sh_name]) {
        printf("%s", &strtab[shdr->sh_name]);
    }
    else {
        printf("(NULL)");
    }
    putchar('\n');

    printf("sh_type     :");
    switch (shdr->sh_type) {
    case SHT_NULL:
        printf("NULL - unused.");
        break;
    case SHT_PROGBITS:
        printf("PROGBITS - Program data.");
        break;
    case SHT_SYMTAB:
        printf("SYMTAB - Symbol table.");
        break;
    case SHT_STRTAB:
        printf("STRTAB - String table.");
        break;
    case SHT_RELA:
        printf("RELA - Relocation entries with addends.");
        break;
    case SHT_HASH:
        printf("HASH - Symbol hash table.");
        break;
    case SHT_DYNAMIC:
        printf("DYNAMIC - Dynamic linking information.");
        break;
    case SHT_NOTE:
        printf("NOTE - Notes.");
        break;
    case SHT_NOBITS:
        printf("NOBITS - Program space with no data (bss).");
        break;
    case SHT_REL:
        printf("REL - Relocation entries, no addends.");
        break;
    case SHT_SHLIB:
        printf("SHLIB - Reserved.");
        break;
    case SHT_DYNSYM:
        printf("DYNSYM - Dynamic linker symbol table.");
        break;
    case SHT_INIT_ARRAY:
        printf("INIT_ARRAY - Array of constructors.");
        break;
    case SHT_FINI_ARRAY:
        printf("FINI_ARRAY - Array of destructors.");
        break;
    case SHT_PREINIT_ARRAY:
        printf("PREINIT_ARRAY - Array of pre-constructors.");
        break;
    case SHT_GROUP:
        printf("GROUP - Section group.");
        break;
    case SHT_SYMTAB_SHNDX:
        printf("SYMTAB_SHNDX - Extended section indeces.");
        break;
    default:
        printf("Unknown Section Header type.");
        break;
    }
    putchar('\n');

    printf("sh_flags    :");
    {
        if (shdr->sh_flags & SHF_WRITE) {
            printf("write, ");
        };
        if (shdr->sh_flags & SHF_ALLOC) {
            printf("alloc, ");
        };
        if (shdr->sh_flags & SHF_EXECINSTR) {
            printf("execute, ");
        };
        if (shdr->sh_flags & SHF_MERGE) {
            printf("merge, ");
        };
        if (shdr->sh_flags & SHF_STRINGS) {
            printf("strings, ");
        };
        if (shdr->sh_flags & SHF_INFO_LINK) {
            printf("info, ");
        };
        if (shdr->sh_flags & SHF_LINK_ORDER) {
            printf("link order, ");
        };
        if (shdr->sh_flags & SHF_OS_NONCONFORMING) {
            printf("extra OS processing required, ");
        };
        if (shdr->sh_flags & SHF_GROUP) {
            printf("group, ");
        };
        if (shdr->sh_flags & SHF_TLS) {
            printf("TLS, ");
        };
        if (shdr->sh_flags & SHF_COMPRESSED) {
            printf("compressed, ");
        };
        if (shdr->sh_flags & SHF_MASKOS) {
            printf("OS specific, ");
        };
        if (shdr->sh_flags & SHF_MASKPROC) {
            printf("processor specific, ");
        };
        if (shdr->sh_flags & SHF_EXCLUDE) {
            printf("exclude, ");
        };
    }
    putchar('\n');

    printf("sh_addr     :0x%x\n", shdr->sh_addr);
    printf("sh_offset   :0x%x\n", shdr->sh_offset);
    printf("sh_size     :0x%x\n", shdr->sh_size);
    printf("sh_link     :0x%x\n", shdr->sh_link);
    printf("sh_info     :0x%x\n", shdr->sh_info);
    printf("sh_addralign:0x%x\n", shdr->sh_addralign);
    printf("sh_entsize  :0x%x\n\n", shdr->sh_entsize);
}