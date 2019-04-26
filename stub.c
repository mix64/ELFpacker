#include <elf.h>
#include <string.h>

#include "defs.h"

unsigned char decode_stub[] = {
    0x60,                         // pushad
    0xBE, 0xFF, 0xFF, 0xFF, 0xFF, // mov esi, decode_start
    0xB9, 0xFF, 0xFF, 0xFF, 0xFF, // mov ecx, decode_size
    0xB0, 0xFF,                   // mov al, decoder
    0x30, 0x06,                   // xor byte ptr [esi], al (LOOP)
    0x46,                         // inc esi
    0x49,                         // dec ecx
    0x75, 0xFA,                   // jnz LOOP
    0x61,                         // popad
    0x68, 0xFF, 0xFF, 0xFF, 0xFF, // mov eax, OEP
    0xC3                          // ret
};

#define DECODE_START 2
#define DECODE_SIZE 7
#define KEY_OFFSET 12
#define OEP_OFFSET 21

void add_stub(char *addr, uint32_t size)
{
    memset(addr, 0, size);
    memcpy(addr, decode_stub, sizeof(decode_stub));
}

void make_stub(uint32_t addr, uint32_t size, uint32_t entry, uint8_t key)
{
    memcpy(&decode_stub[DECODE_START], &addr, sizeof(int32_t));
    memcpy(&decode_stub[DECODE_SIZE], &size, sizeof(int32_t));
    memcpy(&decode_stub[KEY_OFFSET], &key, sizeof(int8_t));
    memcpy(&decode_stub[OEP_OFFSET], &entry, sizeof(int32_t));
}

uint32_t stub_size() { return sizeof(decode_stub); }
