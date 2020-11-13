/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-12     Meco Man     the first verion
 */
 
#include "os.h"
#include <string.h>
#include <rthw.h>

/**
 * ��RT-Thread������ת��ΪuCOS-III������
 *
 * @param RT-Thread������
 *
 * @return uCOS-III������
 */
OS_ERR rt_err_to_ucosiii(rt_err_t rt_err)
{
    switch(-rt_err)/*RTT���صĴ����붼�Ǵ����ŵ�*/
    {
        /*����RTT�����������ԭ��uCOS-III��������м���*/
        case RT_EOK: /* �޴��� */
            return OS_ERR_NONE;
        case RT_ETIMEOUT:/* ��ʱ���� */
            return OS_ERR_TIMEOUT;
        case RT_EINVAL:/* �Ƿ����� */
            return OS_ERR_OPT_INVALID;        
        case RT_EFULL:/* ��Դ����,�ò�������IPC��ʹ�� */
            return OS_ERR_Q_MAX;        
        /*
            ����uCOS-III�Ĵ���������ϸ����RTT�Ĵ���������Ϊ��ͳ��
            ����RTT������ʹ��uCOS-III�Ĵ��������׼ȷ����
            ��˽����RTT�Ĵ��������¶���(����)uCOS-III�Ĵ�����
        */
        case RT_ERROR:/* ��ͨ����    */
            return OS_ERR_RT_ERROR;
        case RT_EEMPTY:/* ����Դ     */
            return OS_ERR_RT_EEMPTY;
        case RT_ENOMEM:/* ���ڴ�     */
            return OS_ERR_RT_ENOMEM;
        case RT_ENOSYS:/* ϵͳ��֧�� */
            return OS_ERR_RT_ENOSYS;
        case RT_EBUSY:/* ϵͳæ      */
            return OS_ERR_RT_EBUSY;
        case RT_EIO:/* IO ����       */
            return OS_ERR_RT_EIO;
        case RT_EINTR:/* �ж�ϵͳ����*/
            return OS_ERR_RT_EINTR;

        default:
            return OS_ERR_RT_ERROR;
    }
}

/**
 * �õ�������һ����������ȴ�IPC,�������̬(��rt_ipc_list_resume�����ı�)
 *
 * @param ������ͷָ��
 *
 * @return ������
 */
rt_err_t rt_ipc_pend_abort_1 (rt_list_t *list)
{
    struct rt_thread *thread;
    register rt_ubase_t temp;
    OS_TCB  *p_tcb;
    
    temp = rt_hw_interrupt_disable();
    /* get thread entry */
    thread = rt_list_entry(list->next, struct rt_thread, tlist);
    p_tcb = (OS_TCB*)thread;
    /* set error code to RT_ERROR */
    thread->error = -RT_ERROR;
    
    /*��ǵ�ǰ��������ȴ�*/
    p_tcb->PendStatus = OS_STATUS_PEND_ABORT; 
    
    rt_hw_interrupt_enable(temp);
    
    /* resume it */
    rt_thread_resume(thread); 

    return RT_EOK;
}

/**
 * �����еȴ���IPC������ȫ�������ȴ����������̬(��rt_ipc_list_resume_all�����ı�)
 *
 * @param ������ͷָ��
 *
 * @return �����˶��ٸ�����
 */
rt_uint16_t rt_ipc_pend_abort_all (rt_list_t *list)
{
    struct rt_thread *thread;
    register rt_ubase_t temp;
    OS_TCB *p_tcb;
    rt_uint16_t i=0;

    /* wakeup all suspend threads */
    while (!rt_list_isempty(list))
    {
        /* disable interrupt */
        temp = rt_hw_interrupt_disable();

        /* get next suspend thread */
        thread = rt_list_entry(list->next, struct rt_thread, tlist);
        p_tcb = ((OS_TCB*)thread);
        /* set error code to RT_ERROR */
        thread->error = -RT_ERROR;
                
        /*��ǵ�ǰ��������ȴ�*/
        p_tcb->PendStatus = OS_STATUS_PEND_ABORT;
        
        /*
         * resume thread
         * In rt_thread_resume function, it will remove current thread from
         * suspend list
         */
        rt_thread_resume(thread);

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);
        
        i++;
    }

    return i;
}

/**
 * �����еȴ���IPC������ȫ����׼�������̬(��rt_ipc_list_resume_all�����ı�)
 *
 * @param ������ͷָ��
 *
 * @return ������
 */
static rt_err_t rt_ipc_post_all (rt_list_t *list)
{
    struct rt_thread *thread;
    register rt_ubase_t temp;
    
    /* wakeup all suspend threads */
    while (!rt_list_isempty(list))
    {
        /* disable interrupt */
        temp = rt_hw_interrupt_disable();

        /* get next suspend thread */
        thread = rt_list_entry(list->next, struct rt_thread, tlist);       
        
        /*
         * resume thread
         * In rt_thread_resume function, it will remove current thread from
         * suspend list
         */
        rt_thread_resume(thread);

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);
    }

    return RT_EOK;
}

/**
 * This function will wake ALL threads which are WAITTING for semaphores
 *
 * @param sem the semaphore object
 *
 * @return the error code
 */
rt_err_t rt_sem_release_all(rt_sem_t sem)
{
    register rt_ubase_t temp;
    register rt_bool_t need_schedule;

    /* parameter check */
    RT_ASSERT(sem != RT_NULL);
    RT_ASSERT(rt_object_get_type(&sem->parent.parent) == RT_Object_Class_Semaphore);

    need_schedule = RT_FALSE;

    /* disable interrupt */
    temp = rt_hw_interrupt_disable();

    RT_DEBUG_LOG(RT_DEBUG_IPC, ("thread %s releases sem:%s, which value is: %d\n",
                                rt_thread_self()->name,
                                ((struct rt_object *)sem)->name,
                                sem->value));

    if (!rt_list_isempty(&sem->parent.suspend_thread))
    {
        /* resume the suspended thread */
        rt_ipc_post_all(&(sem->parent.suspend_thread));
        need_schedule = RT_TRUE;
    }

    /* enable interrupt */
    rt_hw_interrupt_enable(temp);

    /* resume a thread, re-schedule */
    if (need_schedule == RT_TRUE)
        rt_schedule();

    return RT_EOK;
}

#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
/**
 * msh���uCOS-III���ݲ���Ϣ��ȡ
 */
#if defined RT_USING_FINSH && OS_CFG_DBG_EN > 0u
static void rt_ucosiii_wrapper_info (int argc, char *argv[])
{
#if OS_CFG_STAT_TASK_EN >0u
    OS_CPU_USAGE cpu_usage;
#endif
    OS_TCB *p_tcb;
    OS_TMR *p_tmr;
    OS_SEM *p_sem;
    OS_MUTEX *p_mutex;
    OS_Q *p_q;
    OS_FLAG_GRP *p_flag;
    
    CPU_SR_ALLOC();
    
    if(argc == 1)
    {
        rt_kprintf("invalid parameter,use --help to get more information.\n");
        return;
    }
    
    if(!strcmp((const char *)argv[1],(const char *)"--help"))
    {
        rt_kprintf("-v version\n");
#if OS_CFG_STAT_TASK_EN >0u
        rt_kprintf("-u cpu usage\n");
#endif
        rt_kprintf("-t task\n");
#if OS_CFG_SEM_EN > 0u
        rt_kprintf("-s sem\n");
#endif
#if OS_CFG_MUTEX_EN > 0u
        rt_kprintf("-m mutex\n");
#endif
#if OS_CFG_Q_EN > 0u
        rt_kprintf("-q message queue\n");
#endif
#if OS_CFG_FLAG_EN > 0u
        rt_kprintf("-f event flag\n");
#endif
#if OS_CFG_TMR_EN > 0u
        rt_kprintf("-r timer\n");
#endif
    }
    else if(!strcmp((const char *)argv[1],(const char *)"-v"))
    {
        rt_kprintf("template's version: 3.03.00\n");
        rt_kprintf("compatible version: 3.00 - 3.08\n");
    }      
#if OS_CFG_STAT_TASK_EN >0u
    else if(!strcmp((const char *)argv[1],(const char *)"-u"))
    {
        CPU_CRITICAL_ENTER();
        cpu_usage = OSStatTaskCPUUsage;
        CPU_CRITICAL_EXIT();
        rt_kprintf("CPU Usage: %d.%d%%\n",cpu_usage/100,cpu_usage%100);
    }
#endif
#if OS_CFG_TMR_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-r"))
    {
        CPU_CRITICAL_ENTER();
        p_tmr = OSTmrDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Timer--------------------\n");
        while(p_tmr)
        {
            rt_kprintf("name:%s\n",p_tmr->Tmr.parent.name);
            p_tmr = p_tmr->DbgNextPtr;
        }
        rt_kprintf("\n");
    }
#endif
    else if(!strcmp((const char *)argv[1],(const char *)"-t"))
    {
        CPU_CRITICAL_ENTER();
        p_tcb = OSTaskDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Task---------------------\n");
        while(p_tcb)
        {
            rt_kprintf("name:%s\n",p_tcb->Task.name);
            p_tcb = p_tcb->DbgNextPtr;
        }
        rt_kprintf("\n");        
    }
#if OS_CFG_SEM_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-s"))
    {
        CPU_CRITICAL_ENTER();
        p_sem = OSSemDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Sem----------------------\n");
        while(p_sem)
        {
            rt_kprintf("name:%s\n",p_sem->Sem.parent.parent.name);
            p_sem = p_sem->DbgNextPtr;
        }
        rt_kprintf("\n");          
    }
#endif
#if OS_CFG_MUTEX_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-m"))
    {
        CPU_CRITICAL_ENTER();
        p_mutex = OSMutexDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Mutex--------------------\n");
        while(p_mutex)
        {
            rt_kprintf("name:%s\n",p_mutex->Mutex.parent.parent.name);
            p_mutex = p_mutex->DbgNextPtr;
        }
        rt_kprintf("\n");          
    }
#endif
#if OS_CFG_Q_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-q"))
    {
        CPU_CRITICAL_ENTER();
        p_q = OSQDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III MsgQ---------------------\n");
        while(p_q)
        {
            rt_kprintf("name:%s\n",p_q->Msg.parent.parent.name);
            p_q = p_q->DbgNextPtr;
        }
        rt_kprintf("\n");          
    } 
#endif
#if OS_CFG_FLAG_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-f"))
    {
        CPU_CRITICAL_ENTER();
        p_flag = OSFlagDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Flag---------------------\n");
        while(p_flag)
        {
            rt_kprintf("name:%s\n",p_flag->FlagGrp.parent.parent.name);
            p_flag = p_flag->DbgNextPtr;
        }
        rt_kprintf("\n");          
    }  
#endif    
    else
    {
        rt_kprintf("invalid parameter,use --help to get more information.\n");
    }
}
MSH_CMD_EXPORT_ALIAS(rt_ucosiii_wrapper_info, ucos, get ucos-iii wrapper info);
#endif
#endif

/**
 * �Զ���ʼ��
 * uCOS-III���ݲ�֧�ְ���uCOS-IIIԭ��ĳ�ʼ��������г�ʼ������������Щ�����
 * �û������ֶ���ʼ��uCOS-III���ݲ㣬��Ҫֱ������Ӧ�ò������ģ�飬�����ʹ�ø�
 * �궨�塣��rtconfig.h�ж��屾�궨�����RT-Thread��ʼ����ɲ����뵽main�߳�֮ǰ
 * ���Զ���uCOS-III���ݲ��ʼ����ϣ��û�����Ҫרע��uCOS-III��Ӧ�ü����񼴿ɡ�
 * The wrapper supports uCOS-III standard startup procedure. Alternatively,
 * if you want to run uCOS-III apps directly and ignore the startup procedure, 
 * you can choose this option.
 */
#ifdef PKG_USING_UCOSIII_WRAPPER_AUTOINIT
#include <os_app_hooks.h>
static int rt_ucosiii_autoinit(void)
{
    OS_ERR err;
    
    OSInit(&err);                                   /*uCOS-III����ϵͳ��ʼ��*/
    OSStart(&err);                                  /*��ʼ����uCOS-III����ϵͳ*/
    
    CPU_Init();
    
#if OS_CFG_APP_HOOKS_EN > 0u
    App_OS_SetAllHooks();                           /*���ù��Ӻ���*/
#endif  
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	                /*ͳ������*/    
    OSStatReset(&err);                              /*��λͳ������*/    
#endif
    
    return 0;
}
INIT_COMPONENT_EXPORT(rt_ucosiii_autoinit);
#endif
