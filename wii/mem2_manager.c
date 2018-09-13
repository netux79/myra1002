/**
 * Adopted from WiiMC (Tantric 2009-2012) and WiiFlow (http://code.google.com/p/wiiflow/)
 */

#include <ogc/machine/asm.h>
#include <ogc/lwp_heap.h>
#include <ogc/lwp_threads.h>
#include <ogc/system.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "mem2_manager.h"

// Forbid the use of MEM2 through malloc
uint32_t MALLOC_MEM2 = 0;

/*** from libogc (lwp_heap.inl) ****/

static inline heap_block *__lwp_heap_blockat(heap_block *block, uint32_t offset)
{
   return (heap_block *) ((char *) block + offset);
}

static inline heap_block *__lwp_heap_usrblockat(void *ptr)
{
   uint32_t offset = *(((uint32_t *) ptr) - 1);
   return __lwp_heap_blockat(ptr, -offset + -HEAP_BLOCK_USED_OVERHEAD);
}

static inline bool __lwp_heap_blockin(heap_cntrl *heap, heap_block *block)
{
   return ((uint32_t) block >= (uint32_t) heap->start && (uint32_t) block <= (uint32_t) heap->final);
}

static inline bool __lwp_heap_blockfree(heap_block *block)
{
   return !(block->front_flag & HEAP_BLOCK_USED);
}

static inline uint32_t __lwp_heap_blocksize(heap_block *block)
{
   return (block->front_flag & ~HEAP_BLOCK_USED);
}

/*** end from libogc (lwp_heap.inl)  ****/

static uint32_t __lwp_heap_block_size(heap_cntrl *theheap, void *ptr)
{
   heap_block *block;
   uint32_t dsize, level;
   (void)level;

   _CPU_ISR_Disable(level);
   block = __lwp_heap_usrblockat(ptr);

   if(!__lwp_heap_blockin(theheap, block) || __lwp_heap_blockfree(block))
   {
      _CPU_ISR_Restore(level);
      return 0;
   }

   dsize = __lwp_heap_blocksize(block);
   _CPU_ISR_Restore(level);
   return dsize;
}

#define ROUNDUP32(v) (((uint32_t)(v) + 0x1f) & ~0x1f)

static heap_cntrl gx_mem2_heap;

bool gx_init_mem2()  
{
   uint32_t level;
   _CPU_ISR_Disable(level);

   // BIG NOTE: MEM2 on the Wii is 64MB, but a portion of that is reserved for
   // IOS. libogc by default defines the "safe" area for MEM2 to go from
   // 0x90002000 to 0x933E0000. However, from my testing, I've found I need to
   // reserve about 256KB for stuff like network and USB to work correctly.
   // However, other sources says these functions need at least 0xE0000 bytes,
   // 7/8 of a megabyte, of reserved memory to do this. My initial testing
   // shows that we can work with only 128KB, but we use 256KB becuse testing
   // has shown some stuff being iffy with only 128KB, mainly wiimote stuff.
   // If some stuff mysteriously stops working, try fiddling with this size.
   uint32_t size = SYS_GetArena2Size() - 1024 * 256;

   void *heap_ptr = (void *) ROUNDUP32(((uint32_t) SYS_GetArena2Hi() - size));

   SYS_SetArena2Hi(heap_ptr);
   __lwp_heap_init(&gx_mem2_heap, heap_ptr, size, 32);
   _CPU_ISR_Restore(level);
   return true;
}

void *_mem2_memalign(uint8_t align, uint32_t size)
{
   void *ptr;

   if(size == 0)
      return NULL;
 
   ptr = __lwp_heap_allocate(&gx_mem2_heap, size); 

   if (ptr == NULL)
      return NULL;

   return ptr;
}

void *_mem2_malloc(uint32_t size)
{
   return _mem2_memalign(32, size);
}

void _mem2_free(void *ptr)
{ 
   if(!ptr)
      return;

   __lwp_heap_free(&gx_mem2_heap, ptr);
 }

void *_mem2_realloc(void *ptr, uint32_t newsize)
{
   void *newptr = NULL;

   if (ptr == NULL)
      return _mem2_malloc(newsize);

   if (newsize == 0)
   {
      _mem2_free(ptr);
      return NULL;
   }

   uint32_t size = __lwp_heap_block_size(&gx_mem2_heap, ptr);

   if (size > newsize)
      size = newsize;
   
   newptr = _mem2_malloc(newsize);
   
   if (newptr == NULL)
      return NULL;

   memcpy(newptr, ptr, size);
   _mem2_free(ptr);
   return newptr;
}

void *_mem2_calloc(uint32_t num, uint32_t size)
{
   void *ptr = _mem2_malloc(num * size);

   if (ptr == NULL)
      return NULL;

   memset(ptr, 0, num * size);
   return ptr;
}

char *_mem2_strdup(const char *s)
{
    char *ptr = NULL;
    if (s)
    {
        int len = strlen(s) + 1;
        ptr = _mem2_calloc(1, len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}

char *_mem2_strndup(const char *s, size_t n)
{
    char *ptr = NULL;
    if (s)
    {
        int len = n + 1;
        ptr = _mem2_calloc(1, len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}

uint32_t gx_mem2_used()
{
   heap_iblock info;
   __lwp_heap_getinfo(&gx_mem2_heap, &info);
   return info.used_size;
}

uint32_t gx_mem2_total()
{
   heap_iblock info;
   __lwp_heap_getinfo(&gx_mem2_heap, &info);
   return info.used_size + info.free_size;
}

void *__real_malloc(size_t size);
void *__real_calloc(size_t n, size_t size);
void *__real_memalign(size_t a, size_t size);
void __real_free(void *p);
void *__real_realloc(void *p, size_t size);
void *__real_strdup(const char *s);
void *__real_strndup(const char *s, size_t n);
size_t __real_malloc_usable_size(void *p);

__attribute__ ((used)) void *__wrap_malloc(size_t size)
{
   void *p = __real_malloc(size);
   if (p != 0)
      return p;
   return _mem2_malloc(size);
}

__attribute__ ((used)) void *__wrap_calloc(size_t n, size_t size)
{
   void *p = __real_calloc(n, size);
   if (p != 0)
      return p;
   return _mem2_calloc(n, size);
}

__attribute__ ((used)) void *__wrap_memalign(size_t a, size_t size)
{
   void *p = __real_memalign(a, size);
   if (p != 0)
      return p;
   return _mem2_memalign(a, size);
}

__attribute__ ((used)) void __wrap_free(void *p)
{
   if (!p)
      return;

   if (((uint32_t) p & 0x10000000) != 0)
      _mem2_free(p);
   else
      __real_free(p);
}

__attribute__ ((used)) void *__wrap_realloc(void *p, size_t size)
{
   void *n;
   // ptr from mem2
   if (((uint32_t) p & 0x10000000) != 0)
   {
      n = _mem2_realloc(p, size);
      if (n != 0)
         return n;
      n = __real_malloc(size);
      if (n == 0)
         return 0;
      if (p != 0)
      {
         size_t heap_size = __lwp_heap_block_size(&gx_mem2_heap, p);
         memcpy(n, p, heap_size < size ? heap_size : size);
         _mem2_free(p);
      }
      return n;
   }
   // ptr from malloc
   n = __real_realloc(p, size);
   if (n != 0)
      return n;
   n = _mem2_malloc(size);
   if (n == 0)
      return 0;
   if (p != 0)
   {
      size_t heap_size = __real_malloc_usable_size(p);
      memcpy(n, p, heap_size < size ? heap_size : size);
      __real_free(p);
   }
   return n;
}

__attribute__ ((used)) void *__wrap_strdup(const char *s)
{
   void *p = __real_strdup(s);
   if (p != 0)
      return p;
   return _mem2_strdup(s);
}

__attribute__ ((used)) void *__wrap_strndup(const char *s, size_t n)
{
   void *p = __real_strndup(s, n);
   if (p != 0)
      return p;
   return _mem2_strndup(s, n);
}

__attribute__ ((used)) size_t __wrap_malloc_usable_size(void *p)
{
   if (((uint32_t) p & 0x10000000) != 0)
      return __lwp_heap_block_size(&gx_mem2_heap, p);
   return __real_malloc_usable_size(p);
}
