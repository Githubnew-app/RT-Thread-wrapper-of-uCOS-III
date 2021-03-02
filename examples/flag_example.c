/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-15     Meco Man     the first verion
 */

/*������չʾ�����ʹ��uCOS-III���¼���־��*/

#include <os.h>

#if OS_CFG_FLAG_EN > 0u

#define TASK_PRIORITY         6     /*�������ȼ�*/
#define TASK_STACK_SIZE       80    /*�����ջ��С*/
#define TASK_TIMESLICE        5     /*����ʱ��Ƭ*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask1_TCB;/*������ƿ�*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*�����ջ*/
static OS_TCB  AppTask2_TCB;/*������ƿ�*/

static OS_FLAG_GRP flag;

static void AppTask1 (void *param)
{
    OS_ERR err;

    while(1)
    {
        OSFlagPost(&flag, 0x03, OS_OPT_POST_FLAG_SET, &err);
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*��ʱ500ms*/
    }
}


static void AppTask2 (void *param)
{
    OS_ERR err;

    while(1)
    {
        OSFlagPend(&flag, 0x03, 0,
                    OS_OPT_PEND_FLAG_SET_ALL|OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING,
                    0,&err);
        if(err == OS_ERR_NONE)
        {
            rt_kprintf("OSFlagPend ok!\r\n");
        }
        else
        {
            rt_kprintf("OSFlagPend err!\r\n");
        }
    }
}

void flag_example(void)
{
    OS_ERR err;

    OSFlagCreate(&flag, "flag", 0, &err);       /*�����¼���־��*/

    OSTaskCreate(&AppTask1_TCB,                 /*������ƿ�*/
               (CPU_CHAR*)"AppTask1",           /*��������*/
               AppTask1,                        /*������*/
               0,                               /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               &AppTask1_Stack[0],              /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,              /*�����ջ�����λ*/
               TASK_STACK_SIZE,                 /*�����ջ��С*/
               0,                               /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,                  /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,                               /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task1 create err:%d\n",err);
        }

    OSTaskCreate(&AppTask2_TCB,                 /*������ƿ�*/
               (CPU_CHAR*)"AppTask2",           /*��������*/
               AppTask2,                        /*������*/
               0,                               /*���ݸ��������Ĳ���*/
               TASK_PRIORITY,                   /*�������ȼ�*/
               &AppTask2_Stack[0],              /*�����ջ����ַ*/
               TASK_STACK_SIZE/10,              /*�����ջ�����λ*/
               TASK_STACK_SIZE,                 /*�����ջ��С*/
               0,                               /*�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ*/
               TASK_TIMESLICE,                  /*��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ���*/
               0,                               /*�û�����Ĵ洢��*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*����ѡ��*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task2 create err:%d\n",err);
        }
}

#endif
