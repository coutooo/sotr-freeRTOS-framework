/* 
 * File:   TMan.h
 * Author: rodrigo
 *
 * Created on 24 de Janeiro de 2022, 18:02
 */

#ifndef TMAN_H
#define	TMAN_H

typedef struct _Task Task;

void TMAN_Init(TaskHandle_t handler, int ticks);
void TMAN_Close();
int TMAN_TaskAdd(const signed char * name);
Task* TMAN_GetTask(const signed char * name);
void TMAN_TaskRegisterAttributes(const signed char* name, int period);
void TMAN_TaskWaitPeriod(const signed char* name);
int TMAN_TaskStats(Task task);
void pvTickHandler(void *pvParam);

#endif	/* TMAN_H */

