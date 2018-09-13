#ifndef __SYNC_H_
#define __SYNC_H_

void sync_before_read(void *p, uint32_t len);
void sync_after_write(const void *p, uint32_t len);
void sync_before_exec(const void *p, uint32_t len);

#endif
