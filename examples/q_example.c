/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-23     Meco Man     the first verion
 */

/*������չʾ�����ʹ��uCOS-III����Ϣ����*/

#include <os.h>

#define TASK_PRIORITY         6		/*�������ȼ�*/
#define TASK_STACK_SIZE       80 	/*�����ջ��С*/
#define TASK_TIMESLICE        5		/*����ʱ��Ƭ*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask1_TCB;/*������ƿ�*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask2_TCB;/*������ƿ�*/

static OS_Q msg;/*������Ϣ����*/

static char buffer[128];

static void AppTask1 (void *param)
{
    OS_ERR err;
    rt_uint8_t i = 0;
    
    while(1)
    {
        i++;
        rt_sprintf(buffer,"task1 msg:%d",i);
        
        /*������Ϣ����Ϣ����*/
        OSQPost((OS_Q*		)&msg,		
                (void*		)buffer,
                (OS_MSG_SIZE)rt_strlen(buffer),
                (OS_OPT		)OS_OPT_POST_FIFO,
                (OS_ERR*	)&err);    
        
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("queue post err:%d!\r\n",err);
        }          
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*��ʱ500ms*/
    }
}


static void AppTask2 (void *param)
{
    OS_ERR err;
	char  *p;
	OS_MSG_SIZE size;
    
    while(1)
    {
        /*�����ȴ���Ϣ*/
		p=OSQPend((OS_Q*		)&msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
        if(err==OS_ERR_NONE)
        {
            rt_kprintf("pended: str:%s,size:%d\r\n",p,size);
        }
        else
        {
            rt_kprintf("queue pend err!:%d\r\n",err);
        }           
    }
}


void q_example (void)
{
    OS_ERR err;
    
	OSQCreate ((OS_Q*		)&msg,	
                (CPU_CHAR*	)"msg",	
                (OS_MSG_QTY	)10,/*��Ϣ�������ɵ������Ϣ��*/
                (OS_ERR*	)&err);	  
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("queue create err!:%d\r\n",err);
    }    
    
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
}
