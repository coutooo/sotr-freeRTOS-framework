#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "TMan.h"

/* App includes */
#include "../UART/uart.h"

#define PRIORITY_A      ( tskIDLE_PRIORITY + 1 ) 

int nTasks = 1;
TaskHandle_t xTask;

void TMAN_Init(){
    
}

void TMAN_Close(){
    
}

void TMAN_TaskAdd(){
    nTasks++;
}

void TMAN_TaskRegisterAttributes(){
    
}

void TMAN_TaskWaitPeriod(){
    //criar um tickHandler que a cada tick verifique o estado das tasks e caso
    // ela esteja em ready ativa la, se estiver dependencias de outra task
    // esperar que essa acabe e so depois torna la ready
    
    double tmp = xTaskGetTickCount();
    if (tmp != xTaskGetTickCount()){
        for (int i = 1; i <= nTasks; i++){
            eTaskGetState(xTask);   // dar as taks numeros em vez de nome
            if(eTaskGetState(xTask) == eReady){
                vTaskResume(xTask);
            }
        }
    }          
}
    //semaforos?
    //ver sem prioridades so se elas ja estao ready ou nao

void TMAN_TaskStats(){
    
}
        
