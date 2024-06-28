/*******************************************************************************************************************
* @file    : malloc.c
* 
* @date    : 2024/06/28
*
* @author  : science
*
* @version : V1.0
*
* @brief   : malloc 首次适应算法
*
* @note    : 算法思路:
*            1、申请内存时，先从内存表中查找空闲块，如果有空闲块，则分配该块，并将该块的状态设置为已分配；
*            2、分配内存时，分配的内存块大小为内存块大小的整数倍，并将该块的状态设置为已分配；
*            3、释放内存时，将该块的状态设置为空闲；
*            4、内存表中，每一个块的状态占用2位，分别表示该块的状态，0表示空闲，1表示当前块结束，2表示已分配；
*            5、内存池中，每一个块的大小为32字节；
*            6、内存表中，每4个块用一个字节来表示其状态；
**********************************************************************************************************************/ 
#include "malloc.h"


#define MEM_BLOCK_SIZE    32  //! 每个块的大小--------------------------------------------------------------------------
#define MEM_TABLE_SIZE    MEM_DEVICE_SIZE / MEM_BLOCK_SIZE  //! 分区表大小----------------------------------------------

uint8_t MemoryByte[MEM_DEVICE_SIZE] ; //! 内存池变量数组-----------------------------------------------------------------
uint8_t MemTable[MEM_TABLE_SIZE/4] = {0} ; //! 内存表变量数组------------------------------------------------------------


//! 内存设备结构体变量----------------------------------------------------------------------------------------------------
const mem_device_t MemDev = {
    .pMem      = MemoryByte ,
    .pTable    = MemTable,
    .blockSize = MEM_BLOCK_SIZE,
    .tableSize = MEM_TABLE_SIZE,
} ;

/**
 * @brief  初始化内存池
 * 
 * @param  p : 需要初始化的内存池外设结构体指针 
 */
static void Memory_Init(const mem_device_t * p)
{
    for(uint32_t i = 0 ; i < (p->tableSize / 4) ; i++)
    {
        p->pTable[i] = 0 ;
    }
}

static void Memory_Set_Table_Status(const mem_device_t * p , uint32_t index, uint8_t status)
{
    uint32_t bytes = index / 4 ;
    uint32_t bit = index % 4 ;  

    uint8_t byt = p->pTable[bytes] ;
    byt = byt & ~(0x03 << (bit * 2)) ;
    byt = byt | (status << (bit * 2)) ;

    p->pTable[bytes] = byt ;
}

static uint8_t Memory_Get_Table_Status(const mem_device_t * p , uint32_t index)
{
    uint32_t bytes = index / 4 ;
    uint32_t bit = index % 4 ;  

    uint8_t byt = p->pTable[bytes] ;
    byt = byt >> (bit * 2) ;

    return (byt & 0x03) ; 
}

static void Memory_Set_Malloc_Status(const mem_device_t * p , uint32_t index , uint32_t count)
{
    Memory_Set_Table_Status(p , index , 1) ; 

    while(--count)
    {
        Memory_Set_Table_Status(p , --index , 2) ;
    }
}

static uint32_t Memory_Get_Malloc_Size(const mem_device_t * p , uint32_t index)
{
    uint32_t count = 0 ;

    if(Memory_Get_Table_Status(p , index) == 0) { return 0 ; }

    for(uint32_t i = index ; i < p->tableSize ; ++ i)
    {
        if(Memory_Get_Table_Status(p , i) == 1) { return ++ count ; }

        count ++ ;
    }

    return 0 ;
}

static void * Memory_Malloc(const mem_device_t * p , uint32_t size)
{
    if(size == 0) { return NULL ; }

    uint32_t blockSize = p->blockSize , num = 0 ;

    uint32_t count = (size + blockSize - 1) / blockSize ;

    uint8_t * pDat = 0 ;

    for(uint32_t i = 0 ; i < p->tableSize ; i++)
    {
        if(Memory_Get_Table_Status(p, i) == 0){ num ++ ; }
        else { num = 0 ; }
            
        if(num == 1) { pDat = p->pMem + i * blockSize ; } 

        if(num == count) 
        { 
            Memory_Set_Malloc_Status(p , i , count) ;
            return (void *)pDat ; 
        }
    }

    return NULL ;
}

static void * Memory_Calloc(const mem_device_t * p , uint32_t size)
{
    uint8_t * pDat = Memory_Malloc(p , size) ;

    if(pDat == NULL) { return NULL ; }

    for(uint32_t i = 0 ; i < size ; i++) { pDat[i] = 0 ; }

    return (void *)pDat ;
}

static void Memory_Free(const mem_device_t * p , void * pDat)
{
    if(pDat == NULL) { return ; }

    uint32_t blockSize = p->blockSize ;
    uint32_t tableSize = p->tableSize ;
    int32_t index = (int32_t)((uint8_t *)pDat - (uint8_t *)p->pMem) / blockSize;
    uint8_t isEnd = 0 ;

    if(index < 0 || index >= tableSize) { return ; }

    for(uint32_t i = index ; i < tableSize ; ++ i)
    {
        if(Memory_Get_Table_Status(p , i) == 1) { isEnd = 1 ; } 

        Memory_Set_Table_Status(p , i , 0) ;

        if(isEnd) { break ; }
    }
}

static void * Memory_Realloc(const mem_device_t * p , void * pDat , uint32_t size)
{
    if(pDat == NULL) { return Memory_Malloc(p , size) ; }

    uint32_t blockSize = p->blockSize ;
    int32_t index = (int32_t)((uint8_t *)pDat - (uint8_t *)p->pMem) / blockSize ;

    if(index < 0 || index >= p->tableSize) { return Memory_Malloc(p , size) ; }
    
    uint32_t oldSize = Memory_Get_Malloc_Size(p , index) * blockSize ;

    if(oldSize >= size) { return pDat ; }

    uint8_t * pNewDat = Memory_Malloc(p , size) ;

    if(pNewDat == NULL) { return NULL ; }

    for(uint32_t i = 0 ; i < oldSize ; i++) { pNewDat[i] = ((uint8_t *)pDat)[i] ; }

    Memory_Free(p , pDat) ;

    return (void *)pNewDat ;
}


/**
 * @brief  malloc 内存表初始化
 */
void Malloc_Init(void)
{
    Memory_Init(&MemDev) ;
}


/**
 * @brief  打印内存表
 */
void Malloc_Print_Table(void)
{
    const mem_device_t * p = &MemDev ;

    printf("Memory Table begin:\n") ;
    for(int i = 0 ; i < p->tableSize ; i++)
    {
        printf("%d ", Memory_Get_Table_Status(p , i)) ;
        if(i % 32 == 31) { printf("\n") ; }
    }
    printf("\nMemory Table end !\n") ;
}

/**
 * @brief  申请内存
 * 
 * @param   size 申请大小
 * 
 * @return  申请结果
 */
void * Malloc(uint32_t size)
{
    return Memory_Malloc(&MemDev , size) ;
}

/**
 * @brief  申请内存
 * 
 * @param  num  : 申请数量
 * @param  size : 每个数量大小
 * 
 * @return 申请结果 
 */
void * Calloc(uint32_t num , uint32_t size)
{
    return Memory_Calloc(&MemDev , num * size) ;
}

/**
 * @brief  重新分配内存
 * 
 * @param  pDat : 原内存指针
 * @param  size : 新分配内存大小
 * 
 * @return 申请结果 
 */
void * Realloc(void *pDat , uint32_t size)
{
    return Memory_Realloc(&MemDev , pDat , size) ;
}

/**
 * @brief  释放内存
 * 
 * @param  p  :  释放的内存指针
 */
void Free(void *p)
{
    Memory_Free(&MemDev , p) ;
}