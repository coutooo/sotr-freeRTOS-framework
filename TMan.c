#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "TMan.h"

/* App includes */
#include "../UART/uart.h"


typedef struct _Task 
{
    char* name;
    int id;
    int period;
    int activationtime;         // proxima ativacao
    int deadline_time;
    int deadline;
    char* precedence;             // se vier N == NONE
    char* precedence_done;
    int phase;                  // kinda offset
    int nactivations;          // n de activacoes
    int nmiss;                 // n de deadline misses   se passar deadlines falhou// começar ou acabar depois do deadline ou nao começar 
    TickType_t xLastExecutionTime;
    int status;                         // 0 - Ready, 1 - Running, 2 - Suspended 
    TaskHandle_t handle;
    UBaseType_t prioridade;
};

Task* taskList;
int nTasks, globalTicks;
int tickTMan=0;

void TMAN_Init(int ticks)
{
    //void * pvPortMalloc( size_t xWantedSize ) // alocar memória da heap
    taskList = (Task*) pvPortMalloc(sizeof(Task)*16);
    globalTicks = ticks;
    nTasks = 0;
    
}

void pvTickHandler(void *pvParam)                  
{
    // perco tasks e ve os estados
    const TickType_t periodoo = globalTicks / portTICK_RATE_MS;
    
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    
// divisao do tick com o perido == phase
    for(;;)
    {
        vTaskDelayUntil(&xLastWakeTime,periodoo);
        tickTMan++;
        for(int i = 0; i<nTasks;i++)
        {
            if(tickTMan < taskList[i].phase) continue;
            if(taskList[i].status == 2)// && taskList[i].activationtime < tickTMan)       // ve se esta suspendida e pronta para ativar
            {
                if(taskList[i].phase + taskList[i].deadline + (taskList[i].period * taskList[i].nactivations) < tickTMan )// && (taskList[i].status == 1))
                {
                    taskList[i].nmiss++;
                }
                TMAN_TaskStats(taskList[i].name);       // printar estatisticas
                taskList[i].deadline_time = tickTMan + taskList[i].deadline;   // calcurar proxima deadline.
                taskList[i].status = 0;       // fica ready

            }
            if(taskList[i].status == 0 && taskList[i].precedence_done == 1)        // se tiver ready
            {
                taskList[i].nactivations++;
                taskList[i].activationtime = tickTMan + taskList[i].period+ taskList[i].phase;   // calcula tempo de execucao
                taskList[i].status = 1;         // fica running
                vTaskResume(taskList[i].handle);    // meter a correr
            }
        }

    }
}

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
    
    xTask = xTaskGetHandle(name);
    task.status = 0;
    task.name = name;
    task.handle = xTask;
    task.activationtime = 0;
    task.nactivations = 0;
    task.nmiss = 0;
    taskList[nTasks] = task;
    nTasks++;
    
    uint8_t msg[80];
    
    sprintf(msg, "Task %s added\n\r", name);
    PrintStr(msg);
    
    return 1;
}

Task* TMAN_GetTask(const signed char * name)
{
//    Task* task;
//    task->handle = xTaskGetHandle(name);
    for (int i=0; i < nTasks; i++){
        if (strcmp(taskList[i].name, name) == 0){
            return &taskList[i];
        }
    }
}

void TMAN_TaskRegisterAttributes(const signed char* name, int period,int phase,int deadline,const signed char* precedence)
{
    // especificar a periocidade da task
    Task* task = TMAN_GetTask(name);
    
    task->period = period;
    
    task->phase = phase;
    
    task->deadline = deadline;
    
    task->deadline_time = deadline;
    
    task->precedence = precedence;
    
    const signed char* None = "N";
    
    if(strcmp(precedence, None) == 0)
    {
        task->precedence_done = 1;
    }
    else
    {
        task->precedence_done = 0;
    }
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
    //  tempo-tempo exec > deadline       dead miss ++    vtaskSuspend
    //tickTMan++;
    Task* task = TMAN_GetTask(name); 

    const signed char* None = "N";
    
//    if(tickTMan++ >= task->activationtime)
//    {
    for(int i = 0; i<nTasks;i++)
    {
        if(strcmp(taskList[i].precedence, name) == 0)
        {
            taskList[i].precedence_done = 1;
        }
    }
       //    char* precedence;             // se vier N == NONE
        //char* precedence_done;
//    if(tickTMan > task->deadline_time)
//    {
//        task->nmiss++;
//    }

    if(strcmp(task->precedence, None) == 0)
    {
        task->precedence_done = 1;
    }
    else
    {
        task->precedence_done = 0;
    }
    task->status = 2;           // fica suspended no status
    vTaskSuspend(task->handle); // suspend  
  //  }
}
    //semaforos?
    //ver sem prioridades so se elas ja estao ready ou nao

void TMAN_TaskStats(const signed char* name)
{
    Task* task = TMAN_GetTask(name);
    printf("\nNumber of Activations of task %s: %d\n\r",name,task->nactivations);
    printf("\nNumber of Deadline Misses of task %s: %d\n\r",name,task->nmiss);
}