/*******************************************************************************************************************
* @file    : malloc.h
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
#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <stdint.h>
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

#define MEM_DEVICE_SIZE    40 * 1024


//! 内存设备结构体-------------------------------------------------------------------------------------------------------
typedef struct 
{
    uint8_t * pMem ;          //! 内存池数组指针
    uint8_t * pTable ;        //! 内存表数组指针
    uint16_t blockSize ;      //! 块大小
    uint32_t tableSize ;      //! 块数量
} mem_device_t;


/**
 * @brief  malloc 内存表初始化
 */
void Malloc_Init(void) ;

/**
 * @brief  打印内存表
 */
void Malloc_Print_Table(void) ;

/**
 * @brief  申请内存
 * 
 * @param   size 申请大小
 * 
 * @return  申请结果
 */
void * Malloc(uint32_t size) ;

/**
 * @brief  申请内存
 * 
 * @param  num  : 申请数量
 * @param  size : 每个数量大小
 * 
 * @return 申请结果 
 */
void * Calloc(uint32_t num , uint32_t size) ;

/**
 * @brief  重新分配内存
 * 
 * @param  pDat : 原内存指针
 * @param  size : 新分配内存大小
 * 
 * @return 申请结果 
 */
void * Realloc(void *pDat , uint32_t size) ;

/**
 * @brief  释放内存
 * 
 * @param  p  :  释放的内存指针
 */
void Free(void *p) ;


#endif /* _MALLOC_H_ */