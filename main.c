#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "malloc.h"

int main(void)
{
    Malloc_Init() ;

	uint8_t * p = Malloc(20*1024) ;

	if(p)
	{
		for(int i = 0 ; i < 198 ; i++) { p[i] = i ; }
		printf("p[100] = %d\n" , p[100]) ;
	} 
	else { printf("malloc failed\n") ; return 0 ; }

	//uint8_t * p1 = Malloc(1*1024) ;

	Malloc_Print_Table() ;

    p = Realloc(p , 40*1024) ;

	printf("p[100] = %d\n" , p[100]) ;

	Malloc_Print_Table() ;

	Free(p) ; Malloc_Print_Table() ;


	printf("hello main \n") ;
	
	printf("hello main \n") ;
	
	return 0 ;
}