/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                          Microsoft Windows
*
* Filename      : app_cfg.h
* Version       : V3.20
* Programmer(s) : JBL
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include  <stdio.h>

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_FS_EN    DEF_ENABLED


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                             5
#define  CLK_OS_CFG_TASK_PRIO                            6

#define  OS_TASK_TMR_PRIO                       (OS_LOWEST_PRIO - 2)


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                       1024
#define  CLK_OS_CFG_TASK_STK_SIZE                       512


/*
*********************************************************************************************************
*                                          FS CONFIGURATION
*********************************************************************************************************
*/

#define  APP_CFG_FS_DEV_CNT                               1
#define  APP_CFG_FS_VOL_CNT                               1
#define  APP_CFG_FS_FILE_CNT                              1
#define  APP_CFG_FS_DIR_CNT                               1
#define  APP_CFG_FS_BUF_CNT                       (2 * APP_CFG_FS_VOL_CNT)
#define  APP_CFG_FS_DEV_DRV_CNT                           1
#define  APP_CFG_FS_WORKING_DIR_CNT                       0
#define  APP_CFG_FS_MAX_SEC_SIZE                        512

#define  APP_CFG_FS_IDE_EN                         DEF_DISABLED
#define  APP_CFG_FS_MSC_EN                         DEF_DISABLED
#define  APP_CFG_FS_NAND_EN                        DEF_DISABLED
#define  APP_CFG_FS_NOR_EN                         DEF_DISABLED
#define  APP_CFG_FS_RAM_EN                         DEF_ENABLED
#define  APP_CFG_FS_SD_EN                          DEF_DISABLED
#define  APP_CFG_FS_SD_CARD_EN                     DEF_DISABLED

#define  APP_CFG_FS_RAM_NBR_SECS                    (1 * 1024)
#define  APP_CFG_FS_RAM_SEC_SIZE                          512


/*
*********************************************************************************************************
*                                        uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN      DEF_ENABLED
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN       DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN             DEF_ENABLED

#ifdef USE_EXT_RAM
#define  LIB_MEM_CFG_HEAP_SIZE               1000000L
#else
#define  LIB_MEM_CFG_HEAP_SIZE               10000L
#endif


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                               0
#define  TRACE_LEVEL_INFO                              1
#define  TRACE_LEVEL_DBG                               2
                                                                            /* Choose the level of debug messages                   */
#define  BSP_CFG_TRACE_LEVEL                       TRACE_LEVEL_OFF
#define  APP_CFG_TRACE_LEVEL                       TRACE_LEVEL_INFO


#define  APP_CFG_TRACE                              printf

#define  APP_TRACE_INFO(x)               ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                      CONFIGURATION ERRORS
*********************************************************************************************************
*/

#endif
