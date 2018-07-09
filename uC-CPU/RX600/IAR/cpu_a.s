;********************************************************************************************************
;                                                uC/CPU
;                                    CPU CONFIGURATION & PORT LAYER
;
;                          (c) Copyright 2004-2010; Micrium, Inc.; Weston, FL
;
;               All rights reserved.  Protected by international copyright laws.
;
;               uC/CPU is provided in source form to registered licensees ONLY.  It is 
;               illegal to distribute this source code to any third party unless you receive 
;               written permission by an authorized Micrium representative.  Knowledge of 
;               the source code may NOT be used to develop a similar product.
;
;               Please help us continue to provide the Embedded community with the finest 
;               software available.  Your honesty is greatly appreciated.
;
;               You can contact us at www.micrium.com.
;********************************************************************************************************

;********************************************************************************************************
;
;                                            CPU PORT FILE
;
;                              Renesas RX600 Series (RX62N) Specific code
;                                       IAR Renesas RX Compiler
;
; Filename      : cpu_a.s
; Version       : V1.29.00.00
; Programmer(s) : FGK
;********************************************************************************************************


;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

        section .text:CODE:NOROOT
	public     _set_ipl
	public     _get_ipl
	code

;********************************************************************************************************
;                                        set_ipl() & get_ipl()
;
; Description: Set or retrieve interrupt priority level.
;********************************************************************************************************
        
_set_ipl:   
        PUSH.L R2
        MVFC PSW, R2
        SHLL #24, R1
        AND #0xF0FFFFFF, R2
        OR  R1,R2
        MVTC R2, PSW
        RTSD #4, R2-R2
        
        
_get_ipl: 
        MVFC PSW, R1 
        SHLR #24, R1 
        AND  #15, R1 
        MVTIPL #15
        RTS
        
        end


