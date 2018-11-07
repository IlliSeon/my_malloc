#ifndef  __mem_h__
#define  __mem_h__

typedef enum
{
	NOERROR=0,
    PARAM_ILLEGALITY,	

}error_t;
#define  KEIL
#define  MIN_MEM_SIZE     20
#define  MEM_ALIGNMENT    1
#define  LWIP_MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))

void      mem_init(void);
void      memory_init();
void      *mem_malloc(size_t size);
error_t   mem_free(void *p);


#endif