
/*
 * Paulo Pedreiras, Sept/2021
 *
 * FREERTOS demo for ChipKit MAX32 board
 * - Creates two periodic tasks
 * - One toggles Led LD4, other is a long (interfering)task that 
 *      activates LD5 when executing 
 * - When the interfering task has higher priority interference becomes visible
 *      - LD4 does not blink at the right rate
 *
 * Environment:
 * - MPLAB X IDE v5.45
 * - XC32 V2.50
 * - FreeRTOS V202107.00
 *
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include <xc.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "TMan.h"


/* App includes */
#include "../UART/uart.h"

/* Set the tasks' period (in system ticks) */
#define LED_FLASH_PERIOD_MS 	( 250 / portTICK_RATE_MS ) // 
#define INTERF_PERIOD_MS 	( 3000 / portTICK_RATE_MS )

/* Control the load task execution time (# of iterations)*/
/* Each unit corresponds to approx 50 ms*/
#define INTERF_WORKLOAD          ( 20)

/* Priorities of the demo application tasks (high numb. -> high prio.) */
#define PRIORITY_HANDLER 99

#define PRIORITY_A      ( tskIDLE_PRIORITY + 1 )                        
#define PRIORITY_B      ( tskIDLE_PRIORITY + 2 )
#define PRIORITY_C      ( tskIDLE_PRIORITY + 3 )
#define PRIORITY_D      ( tskIDLE_PRIORITY + 1 )                        
#define PRIORITY_E	    ( tskIDLE_PRIORITY + 2 )
#define PRIORITY_F      ( tskIDLE_PRIORITY + 3 )
 

/*
 * Prototypes and tasks
 */

void taskBody(void *pvParam)                  
{
    int iTaskTicks = 0;
    uint8_t mesg[80];
    
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    //const TickType_t freq = pdMS_TO_TICKS(LED_FLASH_PERIOD_MS);
    
    const signed char name = (char*) pvParam;
    
    for(;;) {
        TMAN_TaskWaitPeriod(name); // Add args if needed
        //GET_TICKS
        sprintf("%s, %d\n", name, xLastWakeTime);   //print ?Task Name? and ?Ticks?
        for(int i=0; i<50000; i++) {
            for(int j=0; j<50000; j++){
                //do_some_computation_to_consume_time;
            }
        }   
                
        //OTHER_STUFF (if needed)
    }
    
}

void pvInterfTask(void *pvParam)               
{
    
//    volatile uint32_t counter1, counter2;
//    float x=100.1;
//    
//    TickType_t xLastWakeTime;
//    xLastWakeTime = xTaskGetTickCount();
//    const TickType_t freq = pdMS_TO_TICKS(INTERF_PERIOD_MS);
//   
//    for(;;) {
//        
//        PORTCbits.RC1 = 1;        
//        PrintStr("Interfering task release ...");
//        
//        /* Workload. In this case just spend CPU time ...*/        
//        for(counter1=0; counter1 < INTERF_WORKLOAD; counter1++ )
//            for(counter2=0; counter2 < 0x10200; counter2++ )
//            x=x/3;                
//
//        PrintStr("and termination!\n\r");
//        PORTCbits.RC1 = 0;        
//        
//        vTaskDelayUntil( &xLastWakeTime, freq);
//    }
    
}

/*
 * Create the demo tasks then start the scheduler.
 */
int mainTaskManager( void )
{
    TaskHandle_t handler = NULL;
    // Init UART and redirect stdin/stdot/stderr to UART
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1; // If Led active error initializing UART
        while(1);
    }

     __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
     
    
    /* Welcome message*/
    printf("\n *********************************************\n\r");
    printf("Starting TaskManager \n");
    printf("*********************************************\n\r");
    
    xTaskCreate(pvTickHandler, ( const signed char * const ) "Tick Handler", configMINIMAL_STACK_SIZE, NULL, PRIORITY_HANDLER, &handler);
    TMAN_Init(handler,1000);
    
    /* Create the tasks defined within this file. */
    xTaskCreate(taskBody, ( const signed char * const ) "A", configMINIMAL_STACK_SIZE, (void*) "A", PRIORITY_A, NULL);
    TMAN_TaskAdd("A");
    TMAN_TaskRegisterAttributes("A", 5);
    
//    xTaskCreate(taskBody, ( const signed char * const ) "B", configMINIMAL_STACK_SIZE, NULL, PRIORITY_B, NULL);
//    xTaskCreate(taskBody, ( const signed char * const ) "C", configMINIMAL_STACK_SIZE, NULL, PRIORITY_C, NULL);
//    xTaskCreate(taskBody, ( const signed char * const ) "D", configMINIMAL_STACK_SIZE, NULL, PRIORITY_D, NULL);
//    xTaskCreate(taskBody, ( const signed char * const ) "E", configMINIMAL_STACK_SIZE, NULL, PRIORITY_E, NULL);
//    xTaskCreate(pvInterfTask, ( const signed char * const ) "F", configMINIMAL_STACK_SIZE, NULL, PRIORITY_F, NULL);
    
    /* Finally start the scheduler. */
	vTaskStartScheduler();
    
	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}