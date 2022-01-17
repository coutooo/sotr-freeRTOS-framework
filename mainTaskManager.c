
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


/* App includes */
#include "../UART/uart.h"
//#include <semphr.h>
#include <queue.h>

#define SYSCLK  80000000L // System clock frequency, in Hz
#define PBCLOCK 40000000L // Peripheral Bus Clock frequency, in Hz


/* Set the tasks' period (in system ticks) */
#define ACQ_PERIOD_MS           ( 100 / portTICK_RATE_MS )

/* Control the load task execution time (# of iterations)*/
/* Each unit corresponds to approx 50 ms*/
#define INTERF_WORKLOAD          ( 20)

/* Priorities of the demo application tasks (high numb. -> high prio.) */
#define OUT_PRIORITY        ( tskIDLE_PRIORITY + 1 )                        
#define PROC_PRIORITY	    ( tskIDLE_PRIORITY + 2 )
#define ACQ_PRIORITY        ( tskIDLE_PRIORITY + 3 )


static QueueHandle_t xQueue1;
static QueueHandle_t xQueue2;

struct AMessage
{
   char ucMessageID;
   char ucData[ 20 ];
} xMessage;

//Global variables
float result1;
int result2; 

/*
 * Prototypes and tasks
 */

void taskACQ(void *pvParam)                 
{
    
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(ACQ_PERIOD_MS); 
    
    // Variable declarations;
    float res; // Sampled voltage

    
    // Welcome message
    //printf("Prints voltage at AN0 (Pin 54 of ChipKIT)\n\r");
    
    if(xQueue1 != 0){
        // Main loop
        while (1) {
        
            vTaskDelayUntil(&xLastWakeTime, freq);

            // Get one sample
            IFS1bits.AD1IF = 0; // Reset interrupt flag
            AD1CON1bits.ASAM = 1; // Start conversion
            while (IFS1bits.AD1IF == 0); // Wait fo EOC

            // Convert to 0..3.3V 
            res = (ADC1BUF0 * 3.3) / 1023;
            
            //Convert to temperature
            result1 = (res*100)/3.3;

            // Output result
            printf("Voltage: %f\n\r",result1);
            //printf("Temp:%f",(res-2.7315)/.01); // For a LM335 directly connected
            
            xQueueSend(xQueue1, &result1, 100);
        }

    }
    
}

void taskProc(void *pvParam)                
{
    double samples[5] = {0,0,0,0,0};
    double sum = 0;
    int avg = 0;
    
    if (xQueue1 != 0 && xQueue2 != 0){
        for(;;) {
            if (xQueueReceive(xQueue1, &samples, 100 ) == pdTRUE) {
                
                for(int i = 1; i < 6; i++)
                {
                    samples[i] = samples[i-1];
                }
                       
                samples[0] = result1;
                
                for(int i = 0; i < 5; i++)
                {
                    sum += samples[i];
                }
                
                avg = (int) sum / 5.0;
                
                result2 = avg;
                
                sum = 0;
                
                xQueueSend(xQueue2, &result2, 100);
                
            } else {
                
            }     
            
        }
    }       
    
    
}

void taskOut(void *pvParam)
{
    uint8_t message[80];
    
    if (xQueue2 != 0 ){
        for(;;) {
            if (xQueueReceive(xQueue2, &message, 100) == pdTRUE) {
                sprintf(message, "Average: %d\n", result2);
                PrintStr(message);
                
            } else {
                
            }     
            
        }
    }   
    
}

/*
 * Create the demo tasks then start the scheduler.
 */
int mainTaskManager( void )
{
    // Init UART and redirect stdin/stdot/stderr to UART
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1; // If Led active error initializing UART
        while(1);
    }

     __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
     
     // Disable JTAG interface as it uses a few ADC ports
    DDPCONbits.JTAGEN = 0;
    
    // Initialize ADC module
    // Polling mode, AN0 as input
    // Generic part
    AD1CON1bits.SSRC = 7; // Internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1; //Stop conversion when 1st A/D converter interrupt is generated and clears ASAM bit automatically
    AD1CON1bits.FORM = 0; // Integer 16 bit output format
    AD1CON2bits.VCFG = 0; // VR+=AVdd; VR-=AVss
    AD1CON2bits.SMPI = 0; // Number (+1) of consecutive conversions, stored in ADC1BUF0...ADCBUF{SMPI}
    AD1CON3bits.ADRC = 1; // ADC uses internal RC clock
    AD1CON3bits.SAMC = 16; // Sample time is 16TAD ( TAD = 100ns)
    // Set AN0 as input
    AD1CHSbits.CH0SA = 0; // Select AN0 as input for A/D converter
    TRISBbits.TRISB0 = 1; // Set AN0 to input mode
    AD1PCFGbits.PCFG0 = 0; // Set AN0 to analog mode
    // Enable module
    AD1CON1bits.ON = 1; // Enable A/D module (This must be the ***last instruction of configuration phase***)

    
    /* Welcome message*/
    printf("\n\n *********************************************\n\r");
    printf("Starting FreeRTOS Demo\n\r");
    printf("*********************************************\n\r");
    
    xQueue1 = xQueueCreate(10, sizeof(int));
    xQueue2 = xQueueCreate(10, sizeof(xMessage));
    
    /* Create the tasks defined within this file. */
    xTaskCreate(taskACQ, ( const signed char * const ) "ACQ", configMINIMAL_STACK_SIZE, NULL, ACQ_PRIORITY, NULL);
    xTaskCreate(taskProc, ( const signed char * const ) "Proc", configMINIMAL_STACK_SIZE, NULL, PROC_PRIORITY, NULL);
    xTaskCreate(taskOut, ( const signed char * const ) "Out", configMINIMAL_STACK_SIZE, NULL, OUT_PRIORITY, NULL);
    
    /* Finally start the scheduler. */
	vTaskStartScheduler();
    
	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}