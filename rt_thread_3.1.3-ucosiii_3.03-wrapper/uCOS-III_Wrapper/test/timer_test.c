#include <os.h>

#define THREAD_PRIORITY         5
#define THREAD_STACK_SIZE       256
#define THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread2_stack[THREAD_STACK_SIZE];
OS_TCB thread2;

OS_TMR 	tmr1;		//��ʱ��1

//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr, void *p_arg)
{
//    OS_ERR err;
    rt_kprintf("tmr1_callback\r\n");
   
//    OSTmrDel(&tmr1,&err);
//    if(err != OS_ERR_NONE)
//    {
//        rt_kprintf("err:%d\r\n",err);
//    }
}

/* �߳�2��� */
static void thread2_entry(void *param)
{
    OS_ERR err;
    CPU_STK_SIZE free,used,used_max;
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )20,			//20*10=200ms
                (OS_TICK	 )100,          //100*10=1000ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����   
                
    OSTmrStart(&tmr1,&err);	//������ʱ��1
    while(1)
    {
        OSTaskStkChk(RT_NULL,&free,&used,&used_max,&err);
        rt_kprintf("free:%d,used:%d,used_max:%d\r\n",free,used,used_max);
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
    }
}
rt_err_t _rt_thread_init(struct rt_thread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
                        rt_uint8_t        priority,
                        rt_uint32_t       tick)
{
    return rt_thread_init(thread,name,entry,parameter,stack_start,stack_size,priority,tick);
}                        

void timer_test (void)
{
    OS_ERR err;

	OSTaskCreate(&thread2,		            //������ƿ�
			   (CPU_CHAR*)"thread2", 		//��������
               thread2_entry, 			    //������
               0,					        //���ݸ��������Ĳ���
               THREAD_PRIORITY-1,           //�������ȼ�
               thread2_stack,	            //�����ջ����ַ
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