/*
*********************************************************************************************************
*                                                uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                          (c) Copyright 2004-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/CPU is provided in source form to registered licensees ONLY.  It is 
*               illegal to distribute this source code to any third party unless you receive 
*               written permission by an authorized Micrium representative.  Knowledge of 
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest 
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            CPU PORT FILE
*
*                                               ColdFire
*                                            IAR C Compiler
*
* Filename      : cpu_c.c
* Version       : V1.29.01.00
* Programmer(s) : JJL
*                 FGK
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    MICRIUM_SOURCE
#include  <cpu.h>
#include  <cpu_core.h>


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

CPU_INT32U  CPU_VBR_Ptr;


/*$PAGE*/
/*
*********************************************************************************************************
*                                            CPU_VectGet()
*                                           GET ISR VECTOR
*
* Description : This function is called to get the address of the exception handler specified by 'vect'.
*
* Argument(s) : vect        Vector number to retrieve handler.
*
* Return(s)   : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  *CPU_VectGet (CPU_INT16U vect)
{
    CPU_ADDR  addr;
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    addr = *(CPU_ADDR *)(CPU_VBR_Ptr + (CPU_INT16U)vect * 4);
    CPU_CRITICAL_EXIT();

    return ((void *)addr);
}


/*
*********************************************************************************************************
*                                            CPU_VectSet()
*                                           SET ISR VECTOR
*
* Description : This function is called to set the contents of an exception vector.  The function assumes
*               that the VBR (Vector Base Register) is set to 0x00000000.
*
* Argument(s) : vect        is the vector number.
*
*               addr        is the address of the ISR handler.
*
* Return(s)   : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  CPU_VectSet (CPU_INT16U vect, void (*vect_addr)(void))
{
    CPU_ADDR  *pvect;
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    pvect = (CPU_ADDR *)(CPU_VBR_Ptr + (CPU_INT16U)vect * 4);
   *pvect = (CPU_ADDR  ) vect_addr;
    CPU_CRITICAL_EXIT();
}

