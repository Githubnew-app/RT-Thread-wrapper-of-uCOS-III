/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     Meco Man     the first verion
 */

/*������չʾ�����ʹ��uCOS-III�Ķ�ʱ������ȡ�����ջʹ������Լ���ȡCPUʹ����*/

#include <os.h>

#define TASK_PRIORITY         6		/*�������ȼ�*/
#define TASK_STACK_SIZE       80	/*�����ջ��С*/
#define TASK_TIMESLICE        5		/*����ʱ��Ƭ*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask1_TCB;/*������ƿ�*/

static OS_TMR  CPUUsageTimer;/*CPUʹ���ʲ�����ʱ��*/

/*��ʱ���ص�����*/ 
void tmr1_callback(void *p_tmr, void *p_arg)
{
    rt_kprintf("FROM '%s'-->",((OS_TMR*)p_tmr)->Tmr.parent.name);
#if OS_CFG_STAT_TASK_EN > 0u
    rt_kprintf("CPU usage:%d.%d%%\n",OSStatTaskCPUUsage/100,OSStatTaskCPUUsage%100);
#endif
}

/*������*/
static void AppTask1 (void *param)
{
    OS_ERR err;
    CPU_STK_SIZE free,used;
    
	/*������ʱ��1*/
	OSTmrCreate((OS_TMR		*)&CPUUsageTimer,/*��ʱ�����*/
                (CPU_CHAR	*)"CPUUsageTimer",/*��ʱ������*/ 
                (OS_TICK	 )300,/*300*10=3000ms(10��OS_CFG_TMR_TASK_RATE_HZ����)*/
                (OS_TICK	 )50,/*50*10=500ms(10��OS_CFG_TMR_TASK_RATE_HZ����)*/
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, /*����ģʽ*/
                (OS_TMR_CALLBACK_PTR)tmr1_callback,/*��ʱ��1�ص�����*/
                (void	    *)0,/*����Ϊ0*/
                (OS_ERR	    *)&err);/*���صĴ�����*/
                
    OSTmrStart(&CPUUsageTimer,&err);/*������ʱ��*/  
    
    while(1)
    {
 		OSTaskStkChk(RT_NULL,&free,&used,&err);/*��ȡ��ǰ�����ջ��Ϣ*/ 
    	rt_kprintf("AppTask1 free:%d,used:%d\n",free,used);   
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*��ʱ500ms*/
    }
}

void timer_example (void)
{
    OS_ERR err;

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
            rt_kprintf("task create err:%d\n",err);
        }               
}
