#include <stdio.h>
#include <fstream>
#include <string>
#include <cassert>
#include "jstdint.h"

#define PACKED __attribute__((packed))

struct PACKED ELF64Header {
  u8  MAG[4];
  u8  CLASS;
  u8  DATA;
  u8  VERSION;
  u8  OSABI;
  u8  ABIVERSION;
  u8  PADDING[7];

  // 16 bytes

  u16 type;
  u16 machine;
  u32 version;
  u64 entry; // memory address of ...
  u64 phoff; // program header table offset, usually immediately following the ELF Header
  u64 shoff; // section header table offset
  u32 flags;
  u16 ehsize; // size of this header
  u16 phentsize; // size of program header table
  u16 phnum; // number of entries in program header table
  u16 shentsize; // size of section header table
  u16 shnum; // number of entries in the section header table
  u16 shstrndx; // index of the section header table entry containing section names
};

static_assert(sizeof(ELF64Header) == 0x40);

std::string read_entire_file(const char* path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file) throw std::runtime_error("could not open file");

  size_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string content(size, '\0');
  file.read(&content[0], size);

  return content;
}


int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: usage: %s <file>\n", argv[0]);
    return 1;
  }
  const char* path = argv[1];
  std::string content = read_entire_file(path);

  u64 size = content.size();
  const u8* bytes = (u8*)&content[0];

  assert(size >= sizeof(ELF64Header));

  const ELF64Header* elf_header = (ELF64Header*)bytes;
  (void)elf_header;
}
