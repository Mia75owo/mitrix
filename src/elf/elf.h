#ifndef ELF_H_
#define ELF_H_

// NOTE: Reference:
// https://www.mcs.anl.gov/OpenAD/OpenADFortTkExtendedDox/elf_8h_source.html

#include "util/types.h"

typedef u16 ELF_Half;

typedef u32 ELF_Word;
typedef i32 ELF_Sword;

typedef u64 ELF_Xword;
typedef i64 ELF_Sxword;

// Type of addresses
typedef u32 ELF_Addr;
// Type of file offsets
typedef u32 ELF_Off;
// Type for section indices
typedef u16 ELF_Section;
// Type of symbol indices
typedef u32 ELF_Symndx;

#define ELF_INDENT_COUNT (16)

// ELF file header
typedef struct {
    u8 ident[ELF_INDENT_COUNT];  // Magic number

    ELF_Half type;     // object file type
    ELF_Half machine;  // architecture
    ELF_Word version;  // object file version
    ELF_Addr entry;    // entry point virtual address

    ELF_Off ph_off;  // program header table file offset
    ELF_Off sh_off;  // section header table file offset

    ELF_Word flags;        // processor specific flags
    ELF_Half eh_size;      // ELF header size (bytes)
    ELF_Half ph_entsize;   // program header table entry size
    ELF_Half ph_num;       // program header table entry count
    ELF_Half sh_entsize;   // section header table entry size
    ELF_Half sh_num;       // section header table entry count
    ELF_Half sh_strindex;  // section header string table index
} ELF_Header;

// ELF section header
typedef struct {
    ELF_Word name;       // section name (string table index)
    ELF_Word type;       // section type
    ELF_Word flags;      // section flags
    ELF_Addr addr;       // section virtual addr at execution
    ELF_Off offset;      // section file offset
    ELF_Word size;       // section size (bytes)
    ELF_Word link;       // link to another section
    ELF_Word info;       // additional section info
    ELF_Word addralign;  // section alignment
    ELF_Word entrysize;  // entry size if section holds table
} ELF_SH;

// Legal values for ELF_SH.type
typedef enum {
    SHT_NULL,      // unused
    SHT_PROGBITS,  // program data
    SHT_SYMTAB,    // symbol table
    SHT_STRTAB,    // string table
    SHT_RELA,      // relocation entries with addens
    SHT_HASH,      // symbol hash table
    SHT_DYNAMIC,   // dynamic linking info
    SHT_NOTE,      // notes
    SHT_NOBITS,    // program space with no data (bss)
    SHT_REL,       // relocation entries, no addens
    SHT_SHLIB,     // reserved
    SHT_DYNSYM,    // dynamic linker symbol tables

    SHT_LOPROC = 0x70000000,  // start of processor specific
    SHT_HIPROC = 0x7fffffff,  // end of processor specific
    SHT_LOUSER = 0x80000000,  // start of application specific
    SHT_HIUSER = 0x8fffffff,  // end of application specific
} ELF_SH_Type;

// ELF program segment header
typedef struct {
    ELF_Word type;      // segment type
    ELF_Off offset;     // segment file offset
    ELF_Addr vaddr;     // segment virtual address
    ELF_Addr paddr;     // segment physical address
    ELF_Word filesize;  // segment size in file (bytes)
    ELF_Word memsize;   // segment size in memory (bytes)
    ELF_Word flags;     // segment flags
    ELF_Word align;     // segment alignment
} ELF_PH;

// Legal values for ELF_PH.type
typedef enum {
    PT_NULL,                 // unused
    PT_LOAD,                 // loadable program segment
    PT_DYNSMIC,              // dynamic linking info
    PT_INTERP,               // program interpreter
    PT_NOTE,                 // auxiliary information
    PT_SHLIB,                // reserved
    PT_PHDR,                 // entry for header table itself
    PT_NUM,                  // number of defined types
    PT_LOPROC = 0x70000000,  // start of processor specific
    PT_HIPROC = 0x7fffffff,  // end of processor specific
} ELF_PH_Type;

// Legal values for ELF_PH.flags
typedef enum {
    PF_X = (1 << 0),           // segment is executable
    PF_W = (1 << 1),           // segment is writable
    PF_R = (1 << 2),           // segment is readable
    PF_MASKPROC = 0xf0000000,  // processor specific
} ELF_PH_Flags;

// Symbol
typedef struct {
    ELF_Word name;      // symbol name
    ELF_Addr value;     // symbol value
    ELF_Word size;      // symbol size
    u8 info;            // symbol type and binding
    u8 other;           // unused
    ELF_Half sh_index;  // section index
} ELF_Sym;

// Dynamic section entry
typedef struct {
    ELF_Sword tag;  // dynamic entry type
    union {
        ELF_Word val;
        ELF_Addr ptr;
    } un;
} ELF_Dyn;

// Relocation table entry without addend
typedef struct {
    ELF_Addr offset;  // address
    ELF_Word info;    // relocation type and symbol index
} ELF_Rel;

// Own structure to store ELF
typedef struct {
    u8* mem;
    u8* raw;
    u32 size;

    u32 entry;
    ELF_Header* header;

    ELF_SH* got_section;
    ELF_SH* relocation_section;
} ELFObject;

bool elf_parse(ELFObject* this);
bool elf_load_executable(ELFObject* this);
const char* elf_get_symbol(const ELFObject* this, ELF_SH* symtab,
                           ELF_SH* strtab, u32 index);

#endif
