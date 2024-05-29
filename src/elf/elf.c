#include "elf.h"
#include "memory/memory.h"
#include "memory/pmm.h"
#include "util/debug.h"
#include "util/mem.h"

static const char* get_string(const ELFObject* elf, u32 str_offset,
                              ELF_SH* string_table);
ELF_Sym* find_symbol(ELFObject* elf, ELF_SH* symtab_section,
                     ELF_SH* string_table, const char* symbol);

static bool elf_is_valid(ELF_Header* header) {
    return header->ident[0] == 0x7f && header->ident[1] == 'E' &&
           header->ident[2] == 'L' && header->ident[3] == 'F';
}

static ELF_SH* get_section(const ELFObject* elf, u32 index) {
    return (ELF_SH*)(elf->raw + elf->header->sh_off +
                     index * elf->header->sh_entsize);
}
static const char* get_section_name(const ELFObject* elf, ELF_SH* section) {
    ELF_SH* sh_strtab_section = get_section(elf, elf->header->sh_strindex);
    return get_string(elf, section->name, sh_strtab_section);
}
static const char* get_string(const ELFObject* elf, u32 str_offset,
                              ELF_SH* string_table) {
    const char* strings = (const char*)(elf->raw + string_table->offset);
    return strings + str_offset;
}

const char* elf_get_symbol(const ELFObject* this, ELF_SH* symtab,
                           ELF_SH* strtab, u32 index) {
    ELF_Sym* table = (ELF_Sym*)(this->raw + symtab->offset);
    return get_string(this, table[index].name, strtab);
}

bool elf_parse(ELFObject* this) {
    // Header is at the beginning
    this->header = (ELF_Header*)this->raw;

    if (!elf_is_valid(this->header)) {
        klog("ELF identifier invalid!");
        return false;
    }

    u32 num_sections = this->header->sh_num;

    for (u32 i = 0; i < num_sections; i++) {
        ELF_SH* section = get_section(this, i);
        const char* name = get_section_name(this, section);

        // TODO: implement
        if (strcmp(name, ".dynamic") == 0) {
        } else if (strcmp(name, ".dynsym") == 0) {
        } else if (strcmp(name, ".dynstr") == 0) {
        } else if (strcmp(name, ".got.plt") == 0) {
            this->got_section = section;
        } else if (strcmp(name, ".rel.plt") == 0) {
            this->relocation_section = section;
        }
    }

    return true;
}

bool elf_load_executable(ELFObject* this) {
    assert(this->header);

    if (this->header->type != 0x02) {
        klog("ELF is not an executable");
        return false;
    }

    this->entry = this->header->entry;

    u32 num_segments = this->header->ph_num;
    for (u32 i = 0; i < num_segments; i++) {
        ELF_PH* segment = (ELF_PH*)(this->raw + this->header->ph_off + i * this->header->ph_entsize);

        if (segment->type != PT_LOAD)
            continue;
        if (segment->memsize == 0)
            continue;

        u32 flags = PTE_USER | PTE_OWNER;

        // Writable?
        if (segment->flags & PF_W)
            flags |= PTE_READ_WRITE;

        u32 num_pages = CEIL_DIV(segment->memsize, 0x1000);
        if (segment->vaddr & 0xFFF) {
            // if vaddr is not page aligned we need to map one extra page
            num_pages++;
        }

        u32 virt_addr = (segment->vaddr & ~0xFFF);
        // allocate virtual pages
        for (u32 j = 0; j < num_pages; j++) {
            memory_map_page(virt_addr + j * 0x1000, pmm_alloc_pageframe(), flags);
        }

        memset((void*)virt_addr, 0, num_pages * 0x1000);
        memcpy((void*)segment->vaddr, this->raw + segment->offset, segment->filesize);
    }

    return true;
}
