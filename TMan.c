#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "TMan.h"

/* App includes */
#include "../UART/uart.h"

#define PRIORITY_HANDLER 99
#define PRIORITY_A      ( tskIDLE_PRIORITY + 1 ) 




struct _Task {
    char* name;
    int id;
    int period;
    int deadline;
    int phase;
    int nactivations;          // n de activacoes
    int nmiss;                 // n de deadline misses
    TickType_t xLastExecutionTime;
    char status;
    TaskHandle_t handle;
};


int nTasks = 1;
TaskHandle_t xTask;
_Task* task_list;

void TMAN_Init(){
    //void * pvPortMalloc( size_t xWantedSize ) // alocar memória da heap

    
    struct _Task* TMan = (struct _Task*) pvPortMalloc(sizeof(_Task)*16);
    
    xTaskCreate(...., ( const signed char * const ) "A", configMINIMAL_STACK_SIZE, NULL, PRIORITY_HANDLER, NULL);
}

int TManTicks(){
    int ticks = xTaskGetTickCount();
    
    return ticks*5;
}
void TMAN_Close(){
        //void vPortFree( void * pv ) // libertar bloco de memória previamente alocado.
}

void TMAN_TaskAdd(const signed char * name){
    
    nTasks++;
}

void TMAN_TaskRegisterAttributes(int period,int phase,int deadline,char* precedence,int constraints){
    // especificar a periocidade da task
    
    
}

void TMAN_TaskWaitPeriod(){
    //criar um tickHandler que a cada tick verifique o estado das tasks e caso
    // ela esteja em ready ativa la, se estiver dependencias de outra task
    // esperar que essa acabe e so depois torna la ready
    int tmanticks = TManTicks();
    if (tmanticks  == xTaskGetTickCount())
    {
        // verificar o estado das tasks
    }
    
}
    //semaforos?
    //ver sem prioridades so se elas ja estao ready ou nao

int TMAN_TaskStats(struct _Task task){
    
    return task.nactivations;   // falta nmisses
}
        
