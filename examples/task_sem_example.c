/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-27     Meco Man     the first verion
 */

/*������չʾ�����ʹ��uCOS-III�������ڽ��ź���*/

#include <os.h>

#if OS_CFG_TASK_SEM_EN > 0u

#define THREAD_PRIORITY         6
#define THREAD_STACK_SIZE       80
#define THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread1_stack[THREAD_STACK_SIZE];
static OS_TCB thread1;

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread2_stack[THREAD_STACK_SIZE];
static OS_TCB thread2;

/* �߳�1��� */
static void thread1_entry(void *param)
{
    OS_ERR err;

    while(1) 
    {
        OSTaskSemPost(&thread2,OS_OPT_POST_NONE,&err);
        //OSTaskSemPendAbort(&thread2,OS_OPT_POST_NONE,&err);
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);
    }
}
    

/* �߳�2��� */
static void thread2_entry(void *param)
{
    OS_ERR err;
              
    while(1)
    {
        OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
        if(err==OS_ERR_NONE)
        {
            rt_kprintf("has pended a task sem!\r\n");
        }
        else if(err == OS_ERR_PEND_ABORT)
        {
            rt_kprintf("abort!\r\n");
        }
        else
        {
            rt_kprintf("err:%d\r\n",err);
        }
    }
}
                 

void task_sem_example (void)
{
    OS_ERR err;
        
	OSTaskCreate(&thread2,		            //������ƿ�
			   (CPU_CHAR*)"thread2", 		//��������
               thread2_entry, 			    //������
               0,					        //���ݸ��������Ĳ���
               THREAD_PRIORITY-1,           //�������ȼ�
               &thread2_stack[0],           //�����ջ����ַ
               THREAD_STACK_SIZE/10,	    //�����ջ�����λ
               THREAD_STACK_SIZE,		    //�����ջ��С
               0,					        //�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
               THREAD_TIMESLICE,			//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
               0,					        //�û�����Ĵ洢��
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }   
        
        
	OSTaskCreate(&thread1,		            //������ƿ�
			   (CPU_CHAR*)"thread1", 		//��������
               thread1_entry, 			    //������
               0,					        //���ݸ��������Ĳ���
               THREAD_PRIORITY,             //�������ȼ�
               &thread1_stack[0],           //�����ջ����ַ
               THREAD_STACK_SIZE/10,	    //�����ջ�����λ
               THREAD_STACK_SIZE,		    //�����ջ��С
               0,					        //�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
               THREAD_TIMESLICE,			//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
               0,					        //�û�����Ĵ洢��
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }
}

#endif
