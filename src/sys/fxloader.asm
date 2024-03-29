; File: fxloader.asm
; 05/14/2004

     PW 128         ;Page Width (# of char/line) 
     PL 60          ;Page Length for HP Laser
     INCLIST ON     ;Add Include files in Listing

				;*********************************************
				;Test for Valid Processor defined in -D option
				;*********************************************
	IF	USING_265
	ELSEIF	USING_816
	ELSEIF	USING_CDC16F 
	ELSE
		EXIT         "Not Valid Processor: Use -DUSING_816, etc. ! ! ! ! ! ! ! ! ! ! ! !"
	ENDIF


				;*********************************************
				;Test for Valid 'Model' type defined in -D option
				;*********************************************
	IF	SMALL
	ELSEIF	MEDIUM
	ELSEIF	COMPACT
	ELSEIF	LARGE
	ELSE
		EXIT         "Not Valid 'Model' type - SMALL, MEDIUM, COMPACT, or LARGE: Use -DSMALL, etc. ! ! ! ! ! ! ! ! ! ! ! !"
	ENDIF

    title  "fxos 'C' Startup Program V 1.00 for Large Model W65C816 - fxloader.asm"


; bgnpkhdr
;***************************************************************************
;  FILE_NAME: WDC_CStartup_816.asm
;
;  DATA_RIGHTS: Western Design Center and R & C Services Proprietary
;               Copyright(C) 1980-2003
;               All rights reserved. Reproduction in any manner, 
;               in whole or in part, is strictly prohibited without
;               the prior written approval of R & C Services or 
;               Western Design Center.
;
;               Information contained in this publication regarding
;               device applications and the like is intended through
;               suggestion only and may be superseded by updates.  
;               It is your responsibility to ensure that your application
;               meets with your specifications.  No representation or
;               warranty is given and no liability is assumed by 
;               Western Design Center, Inc. with respect to the accuracy
;               or use of such information, or infringement of patents
;               or other intellectual property rights arising from such
;               use or otherwise.  Use of Western Design Center's products
;               as critical components in life support systems is not
;               authorized except with express written approval by
;               Western Design Center, Inc.'s.  No licenses are conveyed,
;               implicitly or otherwise, under any intellectual property rights.
;
;
;
;  TITLE: WDC_CStartup_816
;
;  DESCRIPTION: This File describes the WDC SDS General purpose ROM startup code.
;
;                Program Flow
;
;                First we make sure we are in native mode.
;                Then we set up the stack pointer.
;                Then we set the data bank register.
;                Then we copy the initialized data.
;                Then we clear out the uninitialized data.
;                Then we call main.
;
;  DEFINED FUNCTIONS:
;          Undefined_IRQ
;                   - Process a Bad Interrupt Vector - send code & Hang!
;
;
;  SPECIAL_CONSIDERATIONS:
;
;
;  SHARED_DATA:
;          None
;
;  GLOBAL_MODULES:
;          None
;
;  LOCAL_MODULES:
;          See above in "DEFINED FUNCTIONS"
;
;  AUTHOR: Jim Goodnow II
;
;  CREATION DATE: March 11,1997
;
;  REVISION HISTORY
;     Name           Date         Description
;     ------------   ----------   ------------------------------------------------
;     R. Greenthal   10/07/2003   1.01 Initial
;
;
;
; NOTE:
;    Change the lines for each version - current version is 1.01
;    See - 
;         title  "WDC 'C' Startup Program V 1.01 for Large Model W65C816 - WDC_CStartup_816.asm"
;
;
;***************************************************************************
;endpkhdr


;***************************************************************************
;                             Include Files
;***************************************************************************
;None

; Set 8-bit accumulator
setas   .macro
        SEP #$20        ; set A short 
        LONGA OFF
        .endm
                
; Set 16-bit accumulator
setal   .macro
        REP #$20        ; set A long 
        LONGA ON
        .endm

;***************************************************************************
;                              Global Modules
;***************************************************************************
;None

;***************************************************************************
;                              External Modules
;***************************************************************************
;None

;***************************************************************************
;                              External Variables
;***************************************************************************
;None


;***************************************************************************
;                               Local Constants
;***************************************************************************
;


;	.page

;***************************************************************************
;                          RAM Section
;***************************************************************************
;   



        sttl "WDC_CStartup_816 Code"
	.page

;***************************************************************************
;***************************************************************************
;                    WDC_CStartup_816 Code Section
;***************************************************************************
;***************************************************************************

****************************************************************
*                                                              *
*   Define a special section in case most of the               *
*      code is not in bank 0.                                  *
*                                                              *
****************************************************************
;           Large   Model Function Calls use module Function_Name  and xdef ~~Function_Name

STACK   EQU   $EF00     ;CHANGE THIS FOR YOUR SYSTEM

STARTUP SECTION OFFSET $7D00

~~START:

START:
	SEI
        CLC                ;clear carry
        XCE                ;clear emulation-Now in Native 16 bit Mode
        REP   #$38         ;16 bit registers and Clear Decimal mode
        LONGI ON
        LONGA ON
        LDA   #STACK       ;get the stack address
        TCS                ;and set the stack to it

****************************************************************
*                                                              *
*   Set up the data bank register. We assume that the data     *
*       section is where we want to point at.                  *
*                                                              *
****************************************************************

        SEP   #$20         ;8 bit accum
        LONGA OFF

        LDA   #^_BEG_DATA  ;get bank of data
        PHA
        PLB                ;set data bank register
        REP   #$20         ;back to 16 bit mode
        LONGA ON

****************************************************************
*                                                              *
*    Next, we want to copy the initialized data from           *
*      ROM to RAM.                                             *
*                                                              *
****************************************************************

        LDA   #_END_DATA-_BEG_DATA ;number of bytes to copy
        BEQ   SKIP         ;if none, just skip
        DEC   A            ;less one for MVN instruction
        LDX   #<_ROM_BEG_DATA ;get source into X
        LDY   #<_BEG_DATA  ;get dest into Y
        MVN   #^_ROM_BEG_DATA,#^_BEG_DATA ;copy bytes
SKIP:

****************************************************************
*                                                              *
*   Now, clear out the uninitialized data area.                *
*    We assume that it is in the same bank as DATA.            *
*                                                              *
****************************************************************

        LDX   #_END_UDATA-_BEG_UDATA  ;get number of bytes to clear
        BEQ   DONE         ;nothing to do
        LDA   #0           ;get a zero for storing
        SEP   #$20         ;do byte at a time
        LDY   #_BEG_UDATA  ;get beginning of zeros

LOOP    STA   |0,Y         ;clear memory
        INY                ;bump pointer
        DEX                ;decrement count
        BNE   LOOP         ;continue till done
        REP   #$20         ;16 bit memory reg
DONE:

****************************************************************
*                                                              *
*   Finally, transfer control to the real program.             *
*                                                              *
****************************************************************
;           Small   Model Function Calls use module Function_Name  and xdef __Function_Name
;           Medium  Model Function Calls use module Function_Name  and xdef ~_Function_Name
;           Large   Model Function Calls use module Function_Name  and xdef ~~Function_Name
;           Compact Model Function Calls use module Function_Name  and xdef _~Function_Name		
		
		NOP
		NOP

		XREF  ~~main      ;change MYSTART to yours
        JSL   ~~main      ;long jump in case not bank 0

	XDEF ~~_exit
~~_exit:
        BRK




        sttl "WDC_CStartup_816 IRQ - Interrupt Handler Code"
	.page

;***************************************************************
;                                                              *
;   Example of transfering control to the real Interrupt       *
;    Handler written in 'C'. Note: save all the registers      *
;                                                              *
;***************************************************************
		XDEF	IRQ

IRQ:	
		
		setal 	
		;Example of needing to read/write to data in another data bank

		phb		; save Data Bank
		phd		; save Direct Page Register
		pha
		phx
		phy

		setas

		setal
		XREF	~~irq_dispatch
		jsl		~~irq_dispatch

		
		setal
		
		ply
		plx
		pla
		pld		; restore Direct Page Reg
		plb		; Restore Data Bank


		
		RTI


NMI:
		RTI


BRK:	

		setal 	
		;Example of needing to read/write to data in another data bank

		phb		; save Data Bank
		phd		; save Direct Page Register
		pha
		phx
		phy

		setas

		setal
		; XREF	~~BRKHandler
		; jsl		~~BRKHandler

		setal
		
		ply
		plx
		pla
		pld		; restore Direct Page Reg
		plb		; Restore Data Bank

		RTI
		
COP:	

		setal 	
		;Example of needing to read/write to data in another data bank

		phb		; save Data Bank
		phd		; save Direct Page Register
		pha
		phx
		phy

		setas

		setal
		; XREF	~~COPHandler
		; jsl		~~COPHandler

		setal
		
		ply
		plx
		pla
		pld		; restore Direct Page Reg
		plb		; Restore Data Bank

		RTI
;************************************************
; Bad Vectors
;************************************************

Undefined_IRQ:
		nop
		bra Undefined_IRQ


;WDCMON_START	EQU	$7E00
;ROMSPACE EQU WDCMON_START-*  ;gives space left in the ROM BEFORE TABLES;
;	IF ROMSPACE<0
		;EXIT         "Not Enough Memory for WDC_Cstartup_02.ASM - bumping into WDCMON! ! ! ! ! ! ! ! ! ! ! !"
	;ENDIF

;        ENDS


        XREF  _BEG_DATA
        XREF  _END_DATA
        XREF  _ROM_BEG_DATA
        XREF  _BEG_UDATA
        XREF  _END_UDATA


	ends


        sttl "WDC_CStartup_816 Shadow Vector Defines"
	.page

;***************************************************************************
;        $7E00 - $7FFF New Expanded Shadow Vector Section Definitions
;***************************************************************************
;***************************************************************************
;***************************************************************************

;	hvectors:	section		; the real reset and interrupt vectors
;
;					;65C816 Vectors
;	org	$18ffe4
;					;Status bit E = 0 (Native mode);
;		dw	Undefined_IRQ	; $FFE4 - COP(816)
;		dw	Undefined_IRQ	; $FFE6 - BRK(816)
;		dw	Undefined_IRQ	; $FFE8 - ABORT(816)
;		dw	Undefined_IRQ	; $FFEA - NMI(816)
;		dw	Undefined_IRQ	; $FFEC - IRQRVD(816)
;		dw	IRQ		; $FFEE - IRQ(816)
;					;Status bit E = 1 (Emulation mode)
;		dw	Undefined_IRQ	; $FFF0 - IRQRVD(816)
;		dw	Undefined_IRQ	; $FFF2 - IRQRVD(816)
;		dw	Undefined_IRQ	; $FFF4 - COP(816Emulation)
;		dw	Undefined_IRQ	; $FFF6 - IRQRVD(816Emulation)
;		dw	Undefined_IRQ	; $FFF8 - ABORT(816Emulation)
;
;					; Common Vectors for all CPUs
;		dw	Undefined_IRQ	; $FFFA -  NMIRQ (ALL)
;		dw	START		; $FFFC -  RESET (ALL)
;		dw	Undefined_IRQ	; $FFFE -  IRQBRK (ALL)
;
;		ends


	        



****************************************************************
*                                                              *
*   This section defines the interrupt and reset vectors.      *
*      The reset vectors always start at FFE4 in bank 0.       *
*      Since the vectors are only 16 bits, they must point     *
*      into bank 0. Replace the "Undefined_IRQ" with any       *
*      routines that handle that interrupt.                    *
*                                                              *
****************************************************************


	vectors:	section		; the real reset and interrupt vectors

					;65C816 Vectors
		org	$ffe4
					;Status bit E = 0 (Native mode)
		dw	COP				; $FFE4 - COP(816)
		dw	BRK		     	; $FFE6 - BRK(816)
		dw	Undefined_IRQ	; $FFE8 - ABORT(816)
		dw	NMI				; $FFEA - NMI(816)
		dw	Undefined_IRQ	; $FFEC - IRQRVD(816)
		dw	IRQ		; $FFEE - IRQ(816)
					;Status bit E = 1 (Emulation mode)
		dw	Undefined_IRQ	; $FFF0 - IRQRVD(816)
		dw	Undefined_IRQ	; $FFF2 - IRQRVD(816)
		dw	Undefined_IRQ	; $FFF4 - COP(816Emulation)
		dw	Undefined_IRQ	; $FFF6 - IRQRVD(816Emulation)
		dw	Undefined_IRQ	; $FFF8 - ABORT(816Emulation)

					; Common Vectors for all CPUs
		dw	Undefined_IRQ	; $FFFA -  NMIRQ (ALL)
		dw	START		; $FFFC -  RESET (ALL)
		dw	Undefined_IRQ	; $FFFE -  IRQBRK (ALL)

		ends


	        end

;**********************************************************************
;	End of WDC_CSTARTUP_C.ASM
;**********************************************************************