#include "stm32f10x.h"
#include "stdio.h"
#include "mem.h"

/*mem_list control header*/
struct  mem_list
{
   struct mem_list  *previous;   /*pointer to previus block control  header*/
   struct mem_list  *next;       /*pointer to next    block* control header*/
   uint8_t     used;
};

static    uint32_t siz_t;

#if defined(KEIL)
extern    uint32_t HEAP$$Base;
extern    uint32_t HEAP$$Limit;

uint32_t  __HEAP_START = (uint32_t)&HEAP$$Base; 
uint32_t  __HEAP_END = (uint32_t)&HEAP$$Limit;
#else /*user defined*/

uint32_t  __HEAP_START=0x20000800;
uint32_t  __HEAP_END  =0x20000E00;

#endif

static   struct  mem_list *ram_start;
static   struct  mem_list *ram_ending;
static   struct  mem_list *ram_free;


void mem_init()
{
	 siz_t=sizeof(struct mem_list);
         ram_start=(struct mem_list *)(__HEAP_START);
	 ram_start->previous=NULL;
	 ram_start->next=(struct mem_list*)((__HEAP_END)-(sizeof(struct mem_list) ));
	 ram_start->used=0; 
	
	 ram_ending=(struct mem_list*)( (__HEAP_END)-( sizeof(struct mem_list) ));
	 ram_ending->previous=(struct mem_list *)(__HEAP_END);
	 ram_ending->next=(struct mem_list *)(__HEAP_END);
	 ram_ending->used=1;
   
	 ram_free=ram_start;
}

void *mem_malloc(size_t size)
{
         struct mem_list *p;
	 struct mem_list *temp1,*temp2;
	
	 if(size==0)
	 {
	    return NULL;	
	 }
	 if(size<MIN_MEM_SIZE)
	 {
	   size=MIN_MEM_SIZE;
	 }
	 if(size>MEM_SIZE)
	 {
	   return NULL;
	 }

	 for(p=ram_free;(size_t)((uint32_t)*&p->next-(uint32_t)*&p)>size;p=p->next)
	 {

	    if(!(p->used) && ( (size_t)((uint32_t)*&p->next-(uint32_t)*&p)>=size) )  /*data block is available*/
			 {
				  /*if the remain data rom is smaller than MIN_MEM_SIZE
				   *we do not creat the block control header ,when next time we free prev block 
                                   *this remained data rom will combine with the prev block we freed.*/	
				 if( ( ( (size_t)(  (uint32_t)*&(p->next)-(uint32_t)*&p ) )-size)>MIN_MEM_SIZE  )  
				 {
			          temp1=(struct mem_list *)((uint32_t)*&p+sizeof(struct mem)+size);/*next block addr is   base_addr+offect
                                                         				 p+ ( size+ sizeof(struct mem_list) )*/
				  temp1->next=p->next;
				  temp1->previous=p;
				  temp1->used=0;
				  ram_free=temp1;
				  p->used=0x01;
				 }
		     	return p;
			 }
	 }
	 return NULL;
}

error_t mem_free(void *p)
{
	error_t  error;
	struct  mem_list *temp1,*temp2;
	error=NOERROR;
	/*check param is illegal*/
	 if((uint32_t)*&p>__HEAP_END ||(uint32_t)*&p<__HEAP_START)
	 {
	    error=PARAM_ILLEGALITY;
		  return  error;
	 }
	 else
	 {
		 if((uint32_t)*&p<(uint32_t)*&ram_free)
		{
			 ram_free=p;
			 ram_free->used=0;/*set this block falg unused*/
			 return error;
		}
	 }
}
