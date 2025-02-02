/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xbasic_types.h"

#include "xparameters.h"
#include "sleep.h"


#include "xscugic.h"
#include "xil_exception.h"
//-----------------for XADC---------------------

#include "xsysmon.h"
#include "xstatus.h"

#define SYSMON_DEVICE_ID 	XPAR_SYSMON_0_DEVICE_ID
#define XSM_SEQ_CH_AUX_MASK	XSM_SEQ_CH_AUX07 | \
				XSM_SEQ_CH_AUX14

int SysMonAuxPolledExample(u16 SysMonDeviceId);
static int SysMonFractionToInt(float FloatNum);



static XSysMon SysMonInst;

int Status;
XSysMon_Config *ConfigPtr;
u16 VAuxRawData[2];
XSysMon *SysMonInstPtr = &SysMonInst;
u32 Index;
u32 ValeurCapteurIR;
//-----------------------------------------------
#define INTC_1 61 // IRQ_F2P[0:0]
#define INTC_2 62 // IRQ_F2P[1:1]

// instance of interrupt controller
static XScuGic intc;


Xint32 *ConsigneG = (Xuint32 *)XPAR_MYCONTROLLER2111_0_S00_AXI_BASEADDR;
Xint32 *ConsigneD = (Xuint32 *)XPAR_MYCONTROLLER2111_1_S00_AXI_BASEADDR;

Xint32 *Lignes = (Xuint32 *)XPAR_RECUP_DONNEE_0_S00_AXI_BASEADDR;

void isr0 (void *intc_inst_ptr);
void isr1 (void *intc_inst_ptr);
void init_interruption(void);
void init_XADC(void);

int main()
{
    init_platform();
    init_interruption();
    init_XADC();
    while(1){


    	if (*(Lignes) == 0x00000005){ //Av
    		avance(200);
    	}
   	else if (*(Lignes) == 0x00000003 || *(Lignes) == 0x00000001){// D
   			tour(50,0); //droite

    	}
    else if(*(Lignes) == 0x00000006  || *(Lignes) == 0x00000004){ //G
    			tour(50,1); //droite
    	}
    	else {


			VAuxRawData[0] = XSysMon_GetAdcData(SysMonInstPtr,XSM_CH_AUX_MIN + 7);
			xil_printf("\r\n LE  VAUX%02d DU DROITE is %0d  \r\n", Index,(int)(VAuxRawData[0]));


			VAuxRawData[1] = XSysMon_GetAdcData(SysMonInstPtr,XSM_CH_AUX_MIN + 14);
			xil_printf("\r\n LE  VAUX%02d DU gauche is %0d \r\n", Index,(int)(VAuxRawData[1]));

			 ValeurCapteurIR = VAuxRawData[0] + VAuxRawData[1];


			if (ValeurCapteurIR > 65000){
				avance(50);
			}
			else if((VAuxRawData[0] > VAuxRawData[1]) && (VAuxRawData[0] > 14000 ))
			{
				tour(40,1); //droite
			}
			else if((VAuxRawData[0] < VAuxRawData[1]) && (VAuxRawData[1] > 14000 ))
			{
				tour(40,0); //gauche
			}
			else {
					arret();
			}
    	}


 		xil_printf("\r\n  %0d \r\n", *(Lignes));
    }
    print("Hello World\n\r");
    print("Successfully ran Hello World application");
    cleanup_platform();
    return 0;


}
void avance(int x){
	*(ConsigneG) = x;
	*(ConsigneD) = x;
}

void tour(int x , int s){
	if (s == 1){
		//Droite

	*(ConsigneG) = -x;
	*(ConsigneD) = x;
		}
	else if (s == 0) {
		//Gauche
	*(ConsigneG) = x;
	*(ConsigneD) = -x;
		}
}

void arret(void ){
	*(ConsigneG) = 0;
	*(ConsigneD) = 0;
}

void marche_arrier(void){
	*(ConsigneG) = -30;
	*(ConsigneD) = -30;
	sleep(0.5);

}
void isr0 (void *intc_inst_ptr) {
	xil_printf("isr0 called\n\r");
	marche_arrier();


}


// interrupt service routine for IRQ_F2P[1:1]
void isr1 (void *intc_inst_ptr) {
    xil_printf("isr1 called\n\r");
    marche_arrier();
}


void init_XADC(void){
	xil_printf("start");
	ConfigPtr = XSysMon_LookupConfig( SYSMON_DEVICE_ID);
	if (ConfigPtr == NULL) {
		xil_printf("11");
	}

	XSysMon_CfgInitialize(SysMonInstPtr, ConfigPtr,ConfigPtr->BaseAddress);

	XSysMon_GetStatus(SysMonInstPtr);
	while ((XSysMon_GetStatus(SysMonInstPtr) & XSM_SR_EOS_MASK) !=
			XSM_SR_EOS_MASK);

}

void init_interruption(void){
	int result;
	XScuGic *intc_instance_ptr = &intc;
	XScuGic_Config *intc_config;

	// get config for interrupt controller
	intc_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if (NULL == intc_config) {
		return XST_FAILURE;
	}

	// initialize the interrupt controller driver
	result = XScuGic_CfgInitialize(intc_instance_ptr, intc_config, intc_config->CpuBaseAddress);

	if (result != XST_SUCCESS) {
		return result;
	}

	// set the priority of IRQ_F2P[0:0] to 0xA0 (highest 0xF8, lowest 0x00) and a trigger for a rising edge 0x3.
	XScuGic_SetPriorityTriggerType(intc_instance_ptr, INTC_1, 0xA0, 0x3);

	// connect the interrupt service routine isr0 to the interrupt controller
	result = XScuGic_Connect(intc_instance_ptr, INTC_1, (Xil_ExceptionHandler)isr0, (void *)&intc);

	if (result != XST_SUCCESS) {
		return result;
	}

	// enable interrupts for IRQ_F2P[0:0]
	XScuGic_Enable(intc_instance_ptr, INTC_1);

	// set the priority of IRQ_F2P[1:1] to 0xA8 (highest 0xF8, lowest 0x00) and a trigger for a rising edge 0x3.
	XScuGic_SetPriorityTriggerType(intc_instance_ptr, INTC_2, 0xA8, 0x3);

	// connect the interrupt service routine isr1 to the interrupt controller
	result = XScuGic_Connect(intc_instance_ptr, INTC_2, (Xil_ExceptionHandler)isr1, (void *)&intc);

	if (result != XST_SUCCESS) {
		return result;
	}

	// enable interrupts for IRQ_F2P[1:1]
	XScuGic_Enable(intc_instance_ptr, INTC_2);

	// initialize the exception table and register the interrupt controller handler with the exception table
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, intc_instance_ptr);

	// enable non-critical exceptions
	Xil_ExceptionEnable();
}
