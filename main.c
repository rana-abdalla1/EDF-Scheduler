
/*
 * FreeRTOS V202212.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"





/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"

/* IGNORE*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SET_BIT(reg,bitN)    ((reg) |= (1<<bitN))     // Sets a bit (bitN) in a register (reg)
#define CLR_BIT(reg,bitN)  ((reg) &= ~(1<<bitN))      // Clears a bit (bitN) in a register (reg)
#define TOG_BIT(reg,bitN) ((reg) ^= (1<<bitN))        // Toggles a bit (bitN) in a register (reg)
#define GET_BIT(reg,bitN)    (1 & ((reg)>>bitN))      // Returns value of bit (bitN) in a register (reg)
typedef struct
{
	portX_t Port;
	pinX_t Pin;
	pinDir_t Direction;
	
}PinConfig_t;


extern PinConfig_t PinConfig_array[];
extern uint16_t PinConfig_array_size;


PinConfig_t PinConfig_array[] = 
							{
								{PORT_0, PIN0, INPUT},
								{PORT_0, PIN1, INPUT},
								{PORT_0, PIN2, OUTPUT},
								{PORT_0, PIN3, OUTPUT},
								{PORT_0, PIN4, OUTPUT},
								{PORT_0, PIN5, OUTPUT},
								{PORT_0, PIN6, OUTPUT},
								{PORT_0, PIN7, OUTPUT},
								{PORT_0, PIN8, OUTPUT},
								{PORT_0, PIN9, OUTPUT},
								{PORT_0, PIN10, OUTPUT},
								{PORT_0, PIN11, OUTPUT},
								{PORT_0, PIN13, OUTPUT},
								{PORT_0, PIN14, OUTPUT},
								{PORT_0, PIN15, OUTPUT},
								
								{PORT_1, PIN0, OUTPUT},
								{PORT_1, PIN1, OUTPUT},
								{PORT_1, PIN2, OUTPUT},
								{PORT_1, PIN3, OUTPUT},
								{PORT_1, PIN4, OUTPUT},
								{PORT_1, PIN5, OUTPUT},
								{PORT_1, PIN6, OUTPUT},
								{PORT_1, PIN7, OUTPUT},
								{PORT_1, PIN8, OUTPUT},
								{PORT_1, PIN9, OUTPUT},
								{PORT_1, PIN10, OUTPUT},
								{PORT_1, PIN11, OUTPUT},
								{PORT_1, PIN13, OUTPUT},
								{PORT_1, PIN14, OUTPUT},
								{PORT_1, PIN15, OUTPUT},
							};

uint16_t PinConfig_array_size = sizeof(PinConfig_array)/sizeof(PinConfig_t);


void GPIO_init(void);
 pinState_t GPIO_read(portX_t PortName, pinX_t pinNum);
void GPIO_write(portX_t PortName, pinX_t PinNum, pinState_t pinState);

void GPIO_init(void)
{
	int i;
	
	for(i = 0; i < PinConfig_array_size;i++)
	{
		switch(PinConfig_array[i].Port)
		{
			case PORT_0:
				if(INPUT == PinConfig_array[i].Direction)
				{
					CLR_BIT(IODIR0, PinConfig_array[i].Pin);
				}
				else if (OUTPUT == PinConfig_array[i].Direction)
				{
					SET_BIT(IODIR0, PinConfig_array[i].Pin);
				}
				else
				{
					
				}
				break;
	
			case PORT_1:
				if(INPUT == PinConfig_array[i].Direction)
				{
					CLR_BIT(IODIR1, PinConfig_array[i].Pin);
				}
				else if (OUTPUT == PinConfig_array[i].Direction)
				{
					SET_BIT(IODIR1, PinConfig_array[i].Pin);
				}
				else
				{

				}
				break;
	
			default:
		}
	}
}


pinState_t GPIO_read(portX_t PortName, pinX_t pinNum)
{
	pinState_t state;
	
	switch(PortName)
	{
		case PORT_0:
			{
				state = (pinState_t) GET_BIT(IOPIN0, pinNum);
				break;
			}

		case PORT_1:
			{
				state = (pinState_t) GET_BIT(IOPIN1, pinNum);
				break;
			}
	}
	
	return state;
}


void GPIO_write(portX_t portName, pinX_t pinNum, pinState_t pinState)
{
	switch(portName)
	{
		case PORT_0:
			if(PIN_IS_LOW == pinState)
			{
				SET_BIT(IOCLR0, pinNum);
			}
			else if (PIN_IS_HIGH == pinState)
			{
				SET_BIT(IOSET0, pinNum);
			}
			else
			{
				
			}
			break;

		case PORT_1:
			if(PIN_IS_LOW == pinState)
			{
				SET_BIT(IOCLR1, pinNum);
			}
			else if (PIN_IS_HIGH == pinState)
			{
				SET_BIT(IOSET1, pinNum);
			}
			else
			{
				
			}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* Tasks Periods */
#define BUTTON_1_PERIOD         50
#define BUTTON_2_PERIOD         50
#define PERIODIC_PERIOD     100
#define UART_PERIOD            20
#define LOAD_1_PERIOD        10
#define LOAD_2_PERIOD        100

/* Tasks Handlers */
TaskHandle_t Button1Handler      = NULL;
TaskHandle_t Button2Handler      = NULL;
TaskHandle_t PeriodicHandler = NULL;
TaskHandle_t UartHandler        = NULL;
TaskHandle_t Load1Handler     = NULL;
TaskHandle_t Load2Handler     = NULL;

/*Queue Handlers*/
QueueHandle_t xQueue1 = NULL;
QueueHandle_t xQueue2 = NULL;
QueueHandle_t xQueue3 = NULL;

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
 
 
 /*Tasks Implemntations*/ 
void Button_1_Monitor( void * pvParameters )
{
	
	pinState_t Button1_New_State;
	pinState_t  Button1_Old_State = GPIO_read(PORT_0 , PIN0);
	TickType_t xLastWakeTime = xTaskGetTickCount();
	signed char Flag = 0;

	for( ;; )
	{
		
		
		Button1_New_State = GPIO_read(PORT_0 , PIN0);
		
		
		if( Button1_New_State == PIN_IS_HIGH &&  Button1_Old_State == PIN_IS_LOW)
		{
			
			Flag = 1;
		}
		else if (Button1_New_State == PIN_IS_LOW &&  Button1_Old_State == PIN_IS_HIGH)
		{
			
			
			Flag = 0;
		}
		else
		{
			Flag = 3;
		}
		
		 Button1_Old_State = Button1_New_State;
		
		
		xQueueOverwrite( xQueue1 , &Flag );

		GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTime , BUTTON_1_PERIOD);
		GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);
		GPIO_write(PORT_0,PIN2,PIN_IS_LOW);

	}
}

void Button_2_Monitor( void * pvParameters )
{

	pinState_t  Button2_Old_State = GPIO_read(PORT_0 , PIN1);
	TickType_t xLastWakeTime = xTaskGetTickCount();
	signed char Flag = 0;
	pinState_t Button2_New_State;

	for( ;; )
	{
		
		Button2_New_State = GPIO_read(PORT_0 , PIN1);
		
		if( Button2_New_State == PIN_IS_HIGH &&  Button2_Old_State == PIN_IS_LOW)
		{
			
			Flag = 1;
		}
		else if (Button2_New_State == PIN_IS_LOW &&  Button2_Old_State == PIN_IS_HIGH)
		{
			
			Flag = 0;
			
		}
		else
		{
			Flag = 3;
		}
		
		 Button2_Old_State = Button2_New_State;
		
		xQueueOverwrite( xQueue2 , &Flag );
			
		GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTime , BUTTON_2_PERIOD);
		GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		GPIO_write(PORT_0,PIN2,PIN_IS_LOW);
	}
}


void Periodic_Transmitter ( void * pvParameters )
{
	TickType_t xLastWakeTime= xTaskGetTickCount();
	
    for( ;; )
    {
        /* Task code goes here. */
			
			vSerialPutString("Repeating String", 15);
			GPIO_write(PORT_0, PIN6, PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTime, 100 );
			GPIO_write(PORT_0, PIN6, PIN_IS_HIGH);
		}
			
			

}
void Uart_Receiver (void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	signed char Button1;
	signed char Button2;
	char Rx[28];
	
	uint8_t i = 0;
	for( ; ; )
	{
		if( xQueueReceive( xQueue1, &Button1 , 0) && Button1 != 3)
		{
			xSerialPutChar('\n');		
			if(Button1==1)
			{
							char String[27]="Button 1 Rising\n";
							vSerialPutString((signed char *) String, strlen(String));
			}
			else if(Button1==0)
			{
							char String[27]="Button 1 Falling\n";
							vSerialPutString((signed char *) String, strlen(String));
			}
		}
		
		
		if( xQueueReceive( xQueue2, &Button2 , 0) && Button2 != 3)
		{
			xSerialPutChar('\n');		
			if(Button2==1)
			{
							char String[27]="Button 2 Rising\n";
							vSerialPutString((signed char *) String, strlen(String));
			}
			else if(Button2==0)
			{
							char String[27]="Button 2 Falling\n";
							vSerialPutString((signed char *) String, strlen(String));
			}
		}
		else
		{
			
		}
		
		if( uxQueueMessagesWaiting(xQueue3) != 0)
		{
			for( i = 0 ; i < 28 ; i++)
			{
				xQueueReceive( xQueue3, (Rx+i) , 0);
			}
			vSerialPutString( (signed char *) Rx, strlen(Rx));
			xQueueReset(xQueue3);
		}
		
		GPIO_write(PORT_0, PIN7, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTime , UART_PERIOD);
		GPIO_write(PORT_0, PIN7, PIN_IS_HIGH);
		GPIO_write(PORT_0,PIN2,PIN_IS_LOW);
	}
}
	




void Load_1_Simulation ( void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint32_t i = 0;
	
	for( ; ; )
	{
		for( i = 0 ; i <= 2000; i++)
		{
		}
		GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTime , LOAD_1_PERIOD);
		GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);

		GPIO_write(PORT_0,PIN2,PIN_IS_LOW);
	}
	vTaskDelayUntil(&xLastWakeTime,10);
}

void Load_2_Simulation ( void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint32_t i = 0;
 
		
	for( ; ; )
	{		
		for( i = 0 ; i <= 4000; i++)
		{
			
		}

		GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		
		GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
	
		GPIO_write(PORT_0,PIN2,PIN_IS_LOW);
	}
	
	vTaskDelayUntil(&xLastWakeTime,100);
}
 
 
 
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
	xQueue1 = xQueueCreate( 1, sizeof(char) );
	xQueue2 = xQueueCreate( 1, sizeof(char) );
	xQueue3 = xQueueCreate( 28, sizeof(char) );

    /* Create Tasks here */
	xTaskPeriodicCreate(
			Button_1_Monitor,                  /* Function that implements the task. */
			"BUTTON 1 MONITOR",                /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Button1Handler,       /* Used to pass out the created task's handle. */
			BUTTON_1_PERIOD);     /* Period for the task */

	xTaskPeriodicCreate(
			Button_2_Monitor,                  /* Function that implements the task. */
			"BUTTON 2 MONITOR",                /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Button2Handler,       /* Used to pass out the created task's handle. */
			BUTTON_2_PERIOD);     /* Period for the task */

	xTaskPeriodicCreate(
			Periodic_Transmitter,               /* Function that implements the task. */
			"PERIODIC TRANSMITTER",             /* Text name for the task. */
			100,                                /* Stack size in womain.crds, not bytes. */
			( void * ) 0,                       /* Parameter passed into the task. */
			1,                                  /* Priority at which the task is created. */
			&PeriodicHandler,   /* Used to pass out the created task's handle. */
			PERIODIC_PERIOD);  /* Period for the task */

	xTaskPeriodicCreate(
			Uart_Receiver,                      /* Function that implements the task. */
			"UART RECEIVER",                    /* Text name for the task. */
			100,                                /* Stack size in words, not bytes. */
			( void * ) 0,                       /* Parameter passed into the task. */
			1,                                  /* Priority at which the task is created. */
			&UartHandler,          /* Used to pass out the created task's handle. */
			UART_PERIOD);         /* Period for the task */

	xTaskPeriodicCreate(
			Load_1_Simulation,                 /* Function that implements the task. */
			"LOAD 1 SIMULATION",               /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Load1Handler,      /* Used to pass out the created task's handle. */
			LOAD_1_PERIOD);	   /* Period for the task */

	xTaskPeriodicCreate(
			Load_2_Simulation,                 /* Function that implements the task. */
			"LOAD 1 SIMULATION",               /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Load2Handler,      /* Used to pass out the created task's handle. */
			LOAD_2_PERIOD); 	 /* Period for the task */

	
		
	/* Now all the tasks have been started - start the scheduler.
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();
	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook (void)
{
	GPIO_write(PORT_0,PIN3,PIN_IS_HIGH);
	GPIO_write(PORT_0,PIN3,PIN_IS_LOW);

}

void vApplicationIdleHook (void)
{
	GPIO_write(PORT_0,PIN2,PIN_IS_HIGH);


}

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000; //20 kHhZ
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/