/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     Meco Man     the first verion
 */

/*������չʾ�����ʹ��uCOS-III�Ļ����������̼߳乲����Դ����*/

#include <os.h>

#define TASK_PRIORITY         5		/*�������ȼ�*/
#define TASK_STACK_SIZE       256	/*�����ջ��С*/
#define TASK_TIMESLICE        5		/*����ʱ��Ƭ*/

static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask1_TCB;/*������ƿ�*/

static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask2_TCB;/*������ƿ�*/

static OS_MUTEX	SyncMutex;/*���廥����*/

/*������*/
static void AppTask1 (void *param)
{
	OS_ERR err;
	
	while(1)
	{
        OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*���뻥���� ������ʹ�����Դ*/ 
        rt_kprintf("This is Task1\r\n");
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*�ͷŻ�����*/
        OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);
	}
}


/*������*/
static void AppTask2 (void *param)
{
	OS_ERR err;
	
	while(1)
	{
		OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*������ʹ�����Դ*/
		rt_kprintf("This is Task2\r\n");
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*�ͷŻ�����*/
        OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err);
	}	
}


void mutex_sample (void)
{
    OS_ERR err;
    
    /*����������*/
	OSMutexCreate ((OS_MUTEX*)&SyncMutex,
                 (CPU_CHAR*	)"SyncMutex",	
                 (OS_ERR*	)&err);    
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("create mutex err!\r\n");
    }
    
	OSTaskCreate(&AppTask1_TCB,		            /*������ƿ�*/ 
			   (CPU_CHAR*)"AppTask1", 		    /*��������*/
               AppTask1, 			            /*������*/
               0,					            /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               AppTask1_Stack,	                /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,	            /*�����ջ�����λ*/
               TASK_STACK_SIZE,		            /*�����ջ��С*/
               0,					            /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,			        /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,					            /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task1 create err:%d\r\n",err);
        }               

	OSTaskCreate(&AppTask2_TCB,		            /*������ƿ�*/ 
			   (CPU_CHAR*)"AppTask2", 		    /*��������*/
               AppTask2, 			            /*������*/
               0,					            /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               AppTask2_Stack,	                /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,	            /*�����ջ�����λ*/
               TASK_STACK_SIZE,		            /*�����ջ��С*/
               0,					            /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,			        /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,					            /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task2 create err:%d\r\n",err);
        }    
}