/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-03     Meco Man     the first verion
 */

/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                EVENT FLAG MANAGEMENT
*
* File    : OS_FLAG.C
* By      : JJL
* Version : V3.03.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#include <os.h>

/*
uCOS-III��֮Ϊ�¼���־��(flag group),RTT��֮Ϊ�¼���(event),����ͳһʹ��"�¼���־��"�ƺ�
*/

/*
����RTTû����ؽӿڣ�������º���û��ʵ��
OSFlagPendAbort
OSFlagPendGetFlagsRdy
*/

/*
************************************************************************************************************************
*                                                 CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : p_grp          is a pointer to the event flag group to create
*
*              p_name         is the name of the event flag group
*
*              flags          contains the initial value to store in the event flag group (typically 0).
*                             -------------˵��-------------
*                             ��uCOS�п������û�ѡ������1Ϊ�¼�����������0Ϊ�¼�������������RTT��ֱ�Ӷ���
*                             ��1Ϊ�¼�����,��˸�λû������,ֱ����0����
*
*              p_err          is a pointer to an error code which will be returned to your application:
*
*                                 OS_ERR_NONE                    if the call was successful.
*                                 OS_ERR_CREATE_ISR              if you attempted to create an Event Flag from an ISR.
*                               - OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Event Flag after you
*                                                                   called OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_OBJ_CREATED             if the event flag group has already been created
*                                 OS_ERR_OBJ_PTR_NULL            if 'p_grp' is a NULL pointer
*                             -------------˵��-------------
*                                 OS_ERR_XXXX        ��ʾ���Լ�������uCOS-IIIԭ��Ĵ�����
*                               - OS_ERR_XXXX        ��ʾ�ô������ڱ����ݲ��Ѿ��޷�ʹ��
*                               + OS_ERR_RT_XXXX     ��ʾ�ô�����Ϊ������RTTר�ô����뼯
*                               Ӧ�ò���Ҫ��API���صĴ������ж�������Ӧ���޸�
*
* Returns    : none
************************************************************************************************************************
*/

void  OSFlagCreate (OS_FLAG_GRP  *p_grp,
                    CPU_CHAR     *p_name,
                    OS_FLAGS      flags,
                    OS_ERR       *p_err)
{
    rt_err_t rt_err;
    
    (void)flags;
    
    /*����Ƿ����ж�������*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_CREATE_ISR;
        return;
    }    
    
    /*����¼���־��ָ���Ƿ�Ϊ��*/
    if(p_grp == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }          
    
    /*����¼���־����ָ���Ƿ�Ϊ��*/
    if(p_name == RT_NULL)
    {
        *p_err = OS_ERR_NAME;
        return;
    }  
    
    /*�ж��ں˶����Ƿ��Ѿ����¼���־�飬���Ƿ��Ѿ�������*/
    if(rt_object_get_type(&p_grp->parent.parent) == RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }   

    /*��uCOS���¼���ֱ�ӱ����뵽����,���������ȼ�����*/
    rt_err = rt_event_init(p_grp,(const char*)p_name,RT_IPC_FLAG_FIFO);
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
}

/*
************************************************************************************************************************
*                                             DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag group.
*
* Arguments  : p_grp     is a pointer to the desired event flag group.
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND           Deletes the event flag group ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS            Deletes the event flag group even if tasks are waiting.
*                                                         In this case, all the tasks pending will be readied.
*                            -------------˵��-------------
*                            ��RTT��û��ʵ��OS_OPT_DEL_NO_PEND
*
*              p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE                  The call was successful and the event flag group was deleted
*                            OS_ERR_DEL_ISR               If you attempted to delete the event flag group from an ISR
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If you didn't pass a pointer to an event flag group
*                            OS_ERR_OPT_INVALID           An invalid option was specified
*                          - OS_ERR_TASK_WAITING          One or more tasks were waiting on the event flag group.
*                        -------------˵��-------------
*                            OS_ERR_XXXX        ��ʾ���Լ�������uCOS-IIIԭ��Ĵ�����
*                          - OS_ERR_XXXX        ��ʾ�ô������ڱ����ݲ��Ѿ��޷�ʹ��
*                          + OS_ERR_RT_XXXX     ��ʾ�ô�����Ϊ������RTTר�ô����뼯
*                          Ӧ�ò���Ҫ��API���صĴ������ж�������Ӧ���޸�
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
*              -------------˵��-------------
*              ����ֵ������,����RTTû��ʵ�ֲ鿴���ź������м����������ڵȴ���API�����ֻ�ܷ���0
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the event flag
*                 group MUST check the return code of OSFlagPost and OSFlagPend().
************************************************************************************************************************
*/

OS_OBJ_QTY  OSFlagDel (OS_FLAG_GRP  *p_grp,
                       OS_OPT        opt,
                       OS_ERR       *p_err)
{
    rt_err_t rt_err;
    
    /*����Ƿ����ж�������*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_DEL_ISR;
        return 0;
    }        
    
    /*���ָ���Ƿ�Ϊ��*/
    if(p_grp == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }
    
    /*�ж��ں˶����Ƿ�Ϊ�¼���־��*/
    if(rt_object_get_type(&p_grp->parent.parent) != RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }   
    
    /*��RTT��û��ʵ��OS_OPT_DEL_NO_PEND*/
    if(opt != OS_OPT_DEL_ALWAYS)
    {
        RT_DEBUG_LOG(RT_DEBUG_UCOSIII,("OSFlagDel: wrapper can't accept this option\r\n"));
        *p_err = OS_ERR_OPT_INVALID;
        return 0;
    }    
    
    rt_err = rt_event_detach(p_grp);
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
    return 0;/*����ֵ������,����RTTû��ʵ�ֲ鿴���¼���־�黹�м����������ڵȴ���API�����ֻ�ܷ���0*/
}

/*
************************************************************************************************************************
*                                             WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag group.  Your
*              application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
*                            The bits you want are specified by setting the corresponding bits in 'flags'.
*                            e.g. if your application wants to wait for bits 0 and 1 then 'flags' would contain 0x03.
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              opt           specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the 'ONE' of the following arguments:
*
*                                OS_OPT_PEND_FLAG_CLR_ALL   You will wait for ALL bits in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_CLR_ANY   You will wait for ANY bit  in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_SET_ALL   You will wait for ALL bits in 'flags' to be set   (1)
*                                OS_OPT_PEND_FLAG_SET_ANY   You will wait for ANY bit  in 'flags' to be set   (1)
*                            -------------˵��-------------
*                               �ڱ����ݲ��У�
*                               OS_OPT_PEND_FLAG_CLR_ALL��OS_OPT_PEND_FLAG_SET_ALL����һ��
*                               OS_OPT_PEND_FLAG_CLR_ANY��OS_OPT_PEND_FLAG_SET_ANY����һ��
*
*                            You can 'ADD' OS_OPT_PEND_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                      the call.  Example, to wait for any flag in a group AND then clear
*                                      the flags that are present, set 'wait_opt' to:
*
*                                      OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
*
*                            You can also 'ADD' the type of pend with 'ONE' of the two option:
*
*                                OS_OPT_PEND_NON_BLOCKING   Task will NOT block if flags are not available
*                                OS_OPT_PEND_BLOCKING       Task will     block if flags are not available
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the event flag group was
*                            posted, aborted or the event flag group deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid and
*                            indicates that you don't need the timestamp.
*                            -------------˵��-------------
*                            �ò�����RTT��û������,��NULL����
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The desired bits have been set within the specified 'timeout'
*                                OS_ERR_OBJ_PTR_NULL        If 'p_grp' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You didn't specify a proper 'opt' argument.
*                              - OS_ERR_PEND_ABORT          The wait on the flag was aborted.
*                                OS_ERR_PEND_ISR            If you tried to PEND from an ISR
*                              - OS_ERR_PEND_WOULD_BLOCK    If you specified non-blocking but the flags were not
*                                                           available.
*                                OS_ERR_SCHED_LOCKED        If you called this function when the scheduler is locked
*                                OS_ERR_TIMEOUT             The bit(s) have not been set in the specified 'timeout'.
*                            -------------˵��-------------
*                                OS_ERR_XXXX        ��ʾ���Լ�������uCOS-IIIԭ��Ĵ�����
*                              - OS_ERR_XXXX        ��ʾ�ô������ڱ����ݲ��Ѿ��޷�ʹ��
*                              + OS_ERR_RT_XXXX     ��ʾ�ô�����Ϊ������RTTר�ô����뼯
*                              Ӧ�ò���Ҫ��API���صĴ������ж�������Ӧ���޸�
*
* Returns    : The flags in the event flag group that made the task ready or, 0 if a timeout or an error
*              occurred.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_TICK       timeout,
                      OS_OPT        opt,
                      CPU_TS       *p_ts,
                      OS_ERR       *p_err)
{
    rt_err_t        rt_err;
    rt_int32_t      time;
    CPU_BOOLEAN     consume;
    OS_OPT          mode;
    rt_uint8_t      rt_option;
    rt_uint32_t     recved;
    
    (void)p_ts;
    
    /*����Ƿ����ж�������*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_PEND_ISR;
        return ((OS_OBJ_QTY)0);
    }       
    
    /*���ָ���Ƿ�Ϊ��*/
    if(p_grp == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return ((OS_OBJ_QTY)0);
    }
    
    /*���������Ƿ���*/
    if(rt_critical_level() > 0)
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return ((OS_OBJ_QTY)0);         
    }   
    
    /*�ж��ں˶����Ƿ�Ϊ�¼���־��*/
    if(rt_object_get_type(&p_grp->parent.parent) != RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return ((OS_OBJ_QTY)0);       
    }  
    
    switch (opt) {                                          /* Validate 'opt'                                         */
        case OS_OPT_PEND_FLAG_CLR_ALL:
        case OS_OPT_PEND_FLAG_CLR_ANY:
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }
    
    /*��ȡopt*/
    if ((opt & OS_OPT_PEND_FLAG_CONSUME) != (OS_OPT)0) {    /* See if we need to consume the flags                    */
        consume = DEF_TRUE;
    } else {
        consume = DEF_FALSE;
    }
    
    mode = opt & OS_OPT_PEND_FLAG_MASK;
    switch (mode) {
        /*OS_OPT_PEND_FLAG_CLR_ALL��OS_OPT_PEND_FLAG_SET_ALL����һ��,�൱��RTT��RT_EVENT_FLAG_AND*/
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_CLR_ALL:    
            rt_option = RT_EVENT_FLAG_AND;
            break;
        
        /*OS_OPT_PEND_FLAG_CLR_ANY��OS_OPT_PEND_FLAG_SET_ANY����һ��,�൱��RTT��RT_EVENT_FLAG_OR*/
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ANY:
            rt_option = RT_EVENT_FLAG_OR;
            break;
             
        default:
            *p_err = OS_ERR_OPT_INVALID;
    }
    
    if (consume == DEF_TRUE)
    {
        /*OS_OPT_PEND_FLAG_CONSUME�൱��RTT�е�RT_EVENT_FLAG_CLEAR*/
        rt_option |= RT_EVENT_FLAG_CLEAR;
    }
    
    /*��RTT��timeoutΪ0��ʾ������,ΪRT_WAITING_FOREVER��ʾ��������,
    ����uCOS-III������ͬ,�����Ҫת��*/
    if((opt & OS_OPT_PEND_NON_BLOCKING) == (OS_OPT)0)
    {
        if(timeout == 0)/*��uCOS-III��timeout=0��ʾ��������*/
        {
            time = RT_WAITING_FOREVER;
        }
        else
        {
            time = timeout;
        }
    }
    else
    {
        time = 0;/*��RTT��timeoutΪ0��ʾ������*/
    }
    
    
    rt_err = rt_event_recv(p_grp,
                           flags,
                           rt_option,
                           time,
                           &recved);
    *p_err = _err_rtt_to_ucosiii(rt_err);
    
    return recved;
}

/*
************************************************************************************************************************
*                                          ABORT WAITING ON AN EVENT FLAG GROUP
*
* Description: This function aborts & readies any tasks currently waiting on an event flag group.  This function should
*              be used to fault-abort the wait on the event flag group, rather than to normally post to the event flag
*              group OSFlagPost().
*
* Arguments  : p_grp     is a pointer to the event flag group
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the event flag
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the event flag
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the event flag group and was
*                                                         readied and informed of the aborted wait; check return value
*                                                         for the number of tasks whose wait on the event flag group
*                                                         was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_grp' is not pointing at an event flag group
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_FLAG_PEND_ABORT_EN > 0u
OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP  *p_grp,
                             OS_OPT        opt,
                             OS_ERR       *p_err)
{
}
#endif

/*
************************************************************************************************************************
*                                       GET FLAGS WHO CAUSED TASK TO BECOME READY
*
* Description: This function is called to obtain the flags that caused the task to become ready to run.
*              In other words, this function allows you to tell "Who done it!".
*
* Arguments  : p_err     is a pointer to an error code
*
*                            OS_ERR_NONE       if the call was successful
*                            OS_ERR_PEND_ISR   if called from an ISR
*
* Returns    : The flags that caused the task to be ready.
************************************************************************************************************************
*/

//OS_FLAGS  OSFlagPendGetFlagsRdy (OS_ERR  *p_err)
//{
//}

/*
************************************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                                OS_OPT_POST_FLAG_SET       set
*                                OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*                             -------------˵��-------------
*                             ��uCOS�п������û�ѡ������1Ϊ�¼�����������0Ϊ�¼�������������RTT��ֱ�Ӷ���
*                             ��1Ϊ�¼�����,��˸�λû������,ֱ����OS_OPT_POST_FLAG_SET����
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The call was successful
*                                OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You specified an invalid option
*                            -------------˵��-------------
*                                OS_ERR_XXXX        ��ʾ���Լ�������uCOS-IIIԭ��Ĵ�����
*                              - OS_ERR_XXXX        ��ʾ�ô������ڱ����ݲ��Ѿ��޷�ʹ��
*                              + OS_ERR_RT_XXXX     ��ʾ�ô�����Ϊ������RTTר�ô����뼯
*                              Ӧ�ò���Ҫ��API���صĴ������ж�������Ӧ���޸�
*
* Returns    : the new value of the event flags bits that are still set.
*
* Note(s)    : 1) The execution time of this function depends on the number of tasks waiting on the event flag group.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPost (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_OPT        opt,
                      OS_ERR       *p_err)
{
    rt_err_t rt_err;
    
    (void)opt;
    
    /*���ָ���Ƿ�Ϊ��*/
    if(p_grp == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }
    
    /*�ж��ں˶����Ƿ�Ϊ�¼���־��*/
    if(rt_object_get_type(&p_grp->parent.parent) != RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }  
    
    rt_err = rt_event_send(p_grp,flags);
    *p_err = _err_rtt_to_ucosiii(rt_err);
    
    return p_grp->set;/*����ִ�к��¼���־���ֵ*/
}