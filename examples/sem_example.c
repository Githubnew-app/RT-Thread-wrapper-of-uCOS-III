/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-22     Meco Man     the first verion
 */

/*
������չʾ�����ʹ��uCOS-III���ź������й㲥ͬ��(RTT�ں�û�иù���)
task1ͬʱ��task2��task3�㲥�ź���
*/

#include <os.h>

#define TASK_PRIORITY         6		/*�������ȼ�*/
#define TASK_STACK_SIZE       80	/*�����ջ��С*/
#define TASK_TIMESLICE        5		/*����ʱ��Ƭ*/


ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask1_TCB;/*������ƿ�*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask2_TCB;/*������ƿ�*/


ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask3_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask3_TCB;/*������ƿ�*/

static OS_SEM sem;

/*����1 ����㲥�ź���*/
static void AppTask1 (void *param)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    
    OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*��ʱ100ms*/
    while(1)
    {
        OSSemPost(&sem,OS_OPT_POST_ALL,&err);
        CPU_CRITICAL_ENTER();
        rt_kprintf("-------------------------\r\n");
        CPU_CRITICAL_EXIT();
        OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*��ʱ800ms*/
    }
}

/*����2 ����ȴ��ź���*/
static void AppTask2 (void *param)
{
    OS_ERR err;
    rt_uint8_t cnt =0;
    CPU_SR_ALLOC();
    
    while(1)
    {
        OSSemPend(&sem,0,OS_OPT_PEND_BLOCKING,0,&err);
        cnt ++;
        if(err == OS_ERR_NONE)
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task2 has pended the sem,cnt:%d\r\n",cnt);
            CPU_CRITICAL_EXIT();
        }
        else
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task2 got a error when are pending a sem\r\n");
            CPU_CRITICAL_EXIT();
        }        
    }
}

/*����3 ����ȴ��ź���*/
static void AppTask3 (void *param)
{
    OS_ERR err;
    rt_uint8_t cnt =0;
    CPU_SR_ALLOC();
    
    while(1)
    {
        OSSemPend(&sem,0,OS_OPT_PEND_BLOCKING,0,&err);
        cnt ++;
        if(err == OS_ERR_NONE)
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task3 has pended the sem,cmt:%d\r\n",cnt);
            CPU_CRITICAL_EXIT();
        }
        else
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task3 got a error when are pending a sem\r\n");
            CPU_CRITICAL_EXIT();
        }
    }
}



void sem_example (void)
{
    OS_ERR err;

    OSSemCreate(&sem,"sem",0,&err);
    
	OSTaskCreate(&AppTask1_TCB,		            /*������ƿ�*/ 
			   (CPU_CHAR*)"AppTask1", 		    /*��������*/
               AppTask1, 			            /*������*/
               0,					            /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               &AppTask1_Stack[0],              /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,	            /*�����ջ�����λ*/
               TASK_STACK_SIZE,		            /*�����ջ��С*/
               0,					            /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,			        /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,					            /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task1 create err:%d\n",err);
        }    

	OSTaskCreate(&AppTask2_TCB,		            /*������ƿ�*/ 
			   (CPU_CHAR*)"AppTask2", 		    /*��������*/
               AppTask2, 			            /*������*/
               0,					            /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               &AppTask2_Stack[0],              /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,	            /*�����ջ�����λ*/
               TASK_STACK_SIZE,		            /*�����ջ��С*/
               0,					            /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,			        /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,					            /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task2 create err:%d\n",err);
        }  


	OSTaskCreate(&AppTask3_TCB,		            /*������ƿ�*/ 
			   (CPU_CHAR*)"AppTask3", 		    /*��������*/
               AppTask3, 			            /*������*/
               0,					            /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               &AppTask3_Stack[0],              /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,	            /*�����ջ�����λ*/
               TASK_STACK_SIZE,		            /*�����ջ��С*/
               0,					            /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,			        /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,					            /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task3 create err:%d\n",err);
        }          
}
