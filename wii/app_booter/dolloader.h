#ifndef _DOLLOADER_H_
#define _DOLLOADER_H_

typedef void (*entrypoint) (void);

uint32_t load_dol_image(const void *dolstart);

#endif
