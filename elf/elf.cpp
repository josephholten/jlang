#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include <cassert>
#include "jstdint.h"

#define PACKED __attribute__((packed))

namespace ELF64 {

static const char ELF_MAG[4] = {0x7f, 'E', 'L', 'F'};

// ELF64 Header
struct PACKED Ehdr {
  char MAG[4];
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

  enum class Class : u8 {
    NONE = 0x00,
    A32,
    A64,
  };

  enum class Data  : u8 {
    NONE = 0x00,
    LSB,
    MSB,
  };

  enum class OSABI : u8 {
    NONE = 0x00,
    SYSV,
    // TODO
  };

  enum class Type : u16 {
    NONE = 0x00,
    REL,
    EXEC,
    DYN,
    CORE,
  };

  enum class Machine : u16 {
    NONE = 0x00,
    // TODO
    X86_64 = 0x3E,
    // TODO
  };
};

static_assert(sizeof(Ehdr) == 0x40);


// Program Header
struct PACKED Phdr {
  u32 type;
  u32 flags;
  u64 offset;
  u64 vaddr;
  u64 paddr;
  u64 filesz;
  u64 memsz;
  u64 align;

  enum class Type {
    NONE = 0x00,
    LOAD,
    DYNAMIC,
    INTERP,
    NOTE,
    SHLIB,
    PHDR,
    TLS,
    LOOS = 0x60000000,
    HIOS = 0x6FFFFFFF,
    LOPROC = 0x70000000,
    HIPROC = 0x7FFFFFFF,
  };

  enum class Flag {
    X = 0x1,
    W = 0x2,
    R = 0x4,
  };
};

// Section Header
struct PACKED Shdr {
  u32 name;
  u32 type;
  u64 flags;
  u64 addr;
  u64 offset;
  u64 size;
  u32 link;
  u32 info;
  u64 addralign;
  u64 entsize;

  enum class Type {
    NONE,
    PROGBITS,
    SYMTAB,
    STRTAB,
    RELA,
    HASH,
    DYNAMIC,
    NOTE,
    NOBITS,
    REL,
    SHLIB,
    DYNSYM,
    INIT_ARRAY,
    FINI_ARRAY,
    PREINIT_ARRAY,
    GROUP,
    SYMTAB_SHNDX,
    NUM,
    LOOS,
  };
};

}


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
  using namespace ELF64;

  if (argc < 2) {
    fprintf(stderr, "ERROR: usage: %s <file>\n", argv[0]);
    return 1;
  }
  const char* path = argv[1];
  std::string content = read_entire_file(path);

  u64 size = content.size();
  const u8* bytes = (u8*)&content[0];

  printf("size = %ld\n", size);

  assert(size >= sizeof(Ehdr));
  const Ehdr* eh = (Ehdr*)bytes;
  assert(0 == strncmp(eh->MAG, ELF_MAG, sizeof(ELF_MAG)));
  assert(eh->version == 1); // current version
  assert(eh->DATA == 1); // little endian
  assert(eh->ehsize == sizeof(Ehdr));

  if (eh->phentsize != 0) {
    assert(size >= eh->phoff + sizeof(Ehdr));
    assert(eh->phentsize == sizeof(Phdr));
    const Phdr* ph_table =  (Phdr*)(bytes+eh->phoff);
    const u16 ph_num = eh->phnum;

    for (u16 i = 0; i < ph_num; i++) {
      const Phdr* ph = ph_table + i;
      printf("program header: %d\n", i);
      printf("  type = %d\n", ph->type);
      (void)ph;
    }
  }

  assert(size >= eh->shoff + sizeof(Shdr));
  assert(eh->shentsize == sizeof(Shdr));
  const Shdr* sh_table =  (Shdr*)(bytes+eh->shoff);
  const u16 sh_num = eh->shnum;
  const u16 sh_names_idx = eh->shstrndx;
  assert(sh_names_idx < sh_num);
  printf("sh names idx: %d\n", sh_names_idx);

  const Shdr* sh_names = sh_table + sh_names_idx;
  const char* section_names = (char*)(bytes + sh_names->offset);

  for (u16 i = 0; i < sh_num; i++) {
    const Shdr* sh = sh_table + i;
    printf("section: %d\n", i);
    printf("  name: %s\n", section_names + sh->name);
  }
}
