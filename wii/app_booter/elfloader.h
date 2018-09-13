#ifndef _ELFLOADER_H_
#define _ELFLOADER_H_

int32_t valid_elf_image (void *addr);
uint32_t load_elf_image (void *addr);

#endif
