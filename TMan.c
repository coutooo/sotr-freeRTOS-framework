#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "TMan.h"

/* App includes */
#include "../UART/uart.h"

#define PRIORITY_HANDLER 99

typedef struct _Task 
{
    char* name;
    int id;
    int period;
    int deadline;
    char* precedence;
    int constraints;
    int phase;
    int nactivations;          // n de activacoes
    int nmiss;                 // n de deadline misses
    TickType_t xLastExecutionTime;
    int status;                         // 0 - Ready, 1 - Running, 2 - Suspended 
    TaskHandle_t handle;
    UBaseType_t prioridade;
};

Task* taskList;
int nTasks, globalTicks;

void TMAN_Init(TaskHandle_t handler, int ticks)
{
    PrintStr("INIT");
    //void * pvPortMalloc( size_t xWantedSize ) // alocar memória da heap
    taskList = (Task*) pvPortMalloc(sizeof(Task)*16);
    globalTicks = ticks;
    nTasks = 0;
    
}

void pvTickHandler(void *pvParam)                  
{
    const TickType_t period = globalTicks/portTICK_RATE_MS;
    int maxPrio;
    int idxAux = 0;
    for(;;)
    {
        maxPrio = 0;
        for (int i = 0; i < (sizeof(taskList)/sizeof(taskList[0])); i++){
            if(taskList[i].status == 1){
                continue;
            } else if(taskList[i].status == 0){
                if (taskList[i].prioridade > maxPrio){
                    //taskList[i]->status = 1;
                    idxAux = i;
                } else {
                    
                }
            }   
        }
        vTaskResume(taskList[idxAux].handle);
        vTaskDelay(period);
    }
}

//int TManTicks()
//{
//    int ticks = xTaskGetTickCount();
//    return ticks*5;
//}

void TMAN_Close()
{
    vPortFree(taskList); // libertar bloco de memória previamente alocado.
    globalTicks = 0;
    nTasks = 0;
    
}

int TMAN_TaskAdd(const signed char * name)
{
    for (int i = 0; i < (sizeof(taskList)/sizeof(taskList[0])); i++){
        if (strcmp(taskList[i].name, name) == 0){
            return 0;
        }
    }
    
    Task task;
    TaskHandle_t xTask;
    
    task.status = 0;
    task.name = name;
    task.handle = xTask;
    //taskList[nTasks] = task;
    nTasks++;
    return 1;
}

Task* TMAN_GetTask(const signed char * name)
{
//    Task* task;
//    task->handle = xTaskGetHandle(name);
    for (int i=0; i < (sizeof(taskList)/sizeof(taskList[0])); i++){
        if (strcmp(taskList[i].name, name) == 0){
            return &taskList[i];
        }
    }
}

void TMAN_TaskRegisterAttributes(const signed char* name, int period)
{
    // especificar a periocidade da task
    Task* task = TMAN_GetTask(name);
    task->period = period;
    //task->phase = phase;
    //task->deadline = deadline;
    //task->precedence = precedence;
    //task->constraints = constraints;
}

void TMAN_TaskWaitPeriod(const signed char* name)
{
    //criar um tickHandler que a cada tick verifique o estado das tasks e caso
    // ela esteja em ready ativa la, se estiver dependente de outra task
    // esperar que essa acabe e so depois torna la ready
    
//    int tmanticks = TManTicks();
//    if (tmanticks  == xTaskGetTickCount())
//    {
//        // verificar o estado das tasks
//        
//    }
    Task* task = TMAN_GetTask(name); 
    if(task->status == 1){
        vTaskSuspend(task->handle);
    }  
}
    //semaforos?
    //ver sem prioridades so se elas ja estao ready ou nao

int TMAN_TaskStats(Task task)
{
    return task.nactivations;   // falta nmisses
}
