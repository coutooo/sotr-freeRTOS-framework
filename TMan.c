#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "TMan.h"

/* App includes */
#include "../UART/uart.h"

#define PRIORITY_HANDLER 99
#define PRIORITY_A      ( tskIDLE_PRIORITY + 1 ) 

int nTasks = 1;
TaskHandle_t xTask;


struct _Task {
    char* Task_name;
    int Task_id;
    int Task_period;
    int Task_deadline;
    int Task_phase;
    int Task_nactivations;          // n de activacoes
    int Task_nmiss;                 // n de deadline misses
    TickType_t xLastExecutionTime;
    char Task_status;
    TaskHandle_t Task_handle;
};
void TMAN_Init(){
    //void * pvPortMalloc( size_t xWantedSize ) // alocar memória da heap

    
    struct _Task* TMan = (struct _Task*) pvPortMalloc(sizeof(_Task));
    
    xTaskCreate(tickANDO, ( const signed char * const ) "B", configMINIMAL_STACK_SIZE, NULL, PRIORITY_HANDLER, NULL);
}

void tickANDO(){
    double tmp = xTaskGetTickCount();
    if (tmp+4 < xTaskGetTickCount()){
        
    }
}
void TMAN_Close(){
        //void vPortFree( void * pv ) // libertar bloco de memória previamente alocado.
}

void TMAN_TaskAdd(const signed char * name,&handler){
    
    struct _Task task;
    task.Task_handle = BaseType_t xTaskCreate(    TaskFunction_t pvTaskCode,
                            const char * const pcName,
                            configSTACK_DEPTH_TYPE usStackDepth,
                            void *pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t *pxCreatedTask
                          );
    xTaskCreate(.....)
    nTasks++;

    strcpy(task.Task_name,name)
    task.Task_handle = xTaskGetHandle(name);
}

void TMAN_TaskRegisterAttributes(int period,int phase,int deadline,char* precedence,constraints){
    // especificar a periocidade da task
}

void TMAN_TaskWaitPeriod(){
    //criar um tickHandler que a cada tick verifique o estado das tasks e caso
    // ela esteja em ready ativa la, se estiver dependencias de outra task
    // esperar que essa acabe e so depois torna la ready
    
    double tmp = xTaskGetTickCount();
    if (tmp == xTaskGetTickCount()){
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
    struct _Task task;
    return task.Task_nactivations;   // falta nmisses
}
        
