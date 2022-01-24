/* 
 * File:   TMan.h
 * Author: rodrigo
 *
 * Created on 24 de Janeiro de 2022, 18:02
 */

#ifndef TMAN_H
#define	TMAN_H


void TMAN_Init(void);
void TMAN_Close(void);
void TMAN_TaskAdd(void);
void TMAN_TaskRegisterAttributes(void);
void TMAN_TaskWaitPeriod(void);
void TMAN_TaskStats(void);

#endif	/* TMAN_H */

