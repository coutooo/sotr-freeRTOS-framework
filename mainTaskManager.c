
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
    uint8_t msg[80];

    const signed char * name = (char*) pvParam;
    
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    
    
    for(;;) {
        TMAN_TaskWaitPeriod(name); // Add args if needed
        sprintf(msg,"%s, %d\n\r", name,xTaskGetTickCount() ); 
        PrintStr(msg);
        for(int i=0; i<5; i++) {
             for(int j=0; j<18; j++){
                 //do_some_computation_to_consume_time;
             }
         }   
    }
    
}
/*
 * Create the demo tasks then start the scheduler.
 */
int mainTaskManager( void )
{
    int t = 1000;
    // Init UART and redirect stdin/stdot/stderr to UART
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1; // If Led active error initializing UART
        while(1);
    }

     __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
     
    
    /* Welcome message*/
    printf("\n *********************************************\n\r");
    printf("Starting TaskManager crl\n");
    printf("*********************************************\n\r");
    
    xTaskCreate(pvTickHandler, ( const signed char * const ) "TickHandler", configMINIMAL_STACK_SIZE, NULL, PRIORITY_A-1, NULL);
    
    TMAN_Init(t);
    
    /* Create the tasks defined within this file. */
    xTaskCreate(taskBody, ( const signed char * const ) "A", configMINIMAL_STACK_SIZE, (void*) "A", PRIORITY_A, NULL);
    TMAN_TaskAdd("A");
    TMAN_TaskRegisterAttributes("A", 5,0,10,"N");        // nome,periodo,phase, deadline, precedence constraints   N = NONE  
    
    xTaskCreate(taskBody, ( const signed char * const ) "B", configMINIMAL_STACK_SIZE, (void*) "B", PRIORITY_B, NULL);
    TMAN_TaskAdd("B");
    TMAN_TaskRegisterAttributes("B", 10,3,14,"A");
    
    xTaskCreate(taskBody, ( const signed char * const ) "C", configMINIMAL_STACK_SIZE, (void*) "C", PRIORITY_C, NULL);
    TMAN_TaskAdd("C");
    TMAN_TaskRegisterAttributes("C", 6,1,5,"N");
    
    xTaskCreate(taskBody, ( const signed char * const ) "D", configMINIMAL_STACK_SIZE, (void*) "D", PRIORITY_D, NULL);
    TMAN_TaskAdd("D");
    TMAN_TaskRegisterAttributes("D", 4,0,10,"N");
    
    xTaskCreate(taskBody, ( const signed char * const ) "E", configMINIMAL_STACK_SIZE, (void*) "E", PRIORITY_E, NULL);
    TMAN_TaskAdd("E");
    TMAN_TaskRegisterAttributes("E", 8,0,20,"F");
    
    xTaskCreate(taskBody, ( const signed char * const ) "F", configMINIMAL_STACK_SIZE, (void*) "F", PRIORITY_F, NULL);
    TMAN_TaskAdd("F");
    TMAN_TaskRegisterAttributes("F", 10,6,30,"N");
    
    /* Finally start the scheduler. */
	vTaskStartScheduler();
    
	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}