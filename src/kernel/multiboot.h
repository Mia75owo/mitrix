#ifndef MULTIBOOT_H_
#define MULTIBOOT_H_

#include "util/types.h"

struct multiboot_aout_symbol_table {
    u32 tabsize;
    u32 strsize;
    u32 addr;
    u32 reserved;
};

struct multiboot_elf_section_header_table {
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
};

struct multiboot_info {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;

    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;

    union {
        struct multiboot_aout_symbol_table aout_sym;
        struct multiboot_elf_section_header_table elf_sec;
    } u;

    u32 mmap_length;
    u32 mmap_addr;

    u32 drives_length;
    u32 drives_addr;

    u32 config_table;
    u32 boot_loader_name;

    u32 apm_table;

    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;

    u64 framebuffer_addr;
    u32 framebuffer_pitch;
    u32 framebuffer_width;
    u32 framebuffer_height;
    u8 framebuffer_bpp;

    // clang-format off
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED  0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB      1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 1
    // clang-format on
    u8 framebuffer_type;

    union {
        struct {
            u32 framebuffer_palette_addr;
            u16 framebuffer_palette_num_colors;
        };
        struct {
            u8 framebuffer_red_field_position;
            u8 framebuffer_red_mask_size;
            u8 framebuffer_green_field_position;
            u8 framebuffer_green_mask_size;
            u8 framebuffer_blue_field_position;
            u8 framebuffer_blue_mask_size;
        };
    };
};

typedef struct {
    u32 size;
    u32 addr_low;
    u32 addr_high;
    u32 len_low;
    u32 len_high;

    // clang-format off
#define MULTIBOOT_MEMORY_AVAILABLE        1
#define MULTIBOOT_MEMORY_RESERVED         2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVC              4
#define MULTIBOOT_MEMORY_BADRAM           5
    // clang-format on
    u32 type;
} __attribute__((packed)) multiboot_mmap_entry;

#endif
