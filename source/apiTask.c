#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "apiRTC.h"
#include <pSystem.h>
#include <apiTask.h>
#include "apiModem.h"
#include <apiDelay.h>
#include "apiModbus.h"



_SYS_TASKS STASK;
_APPLICATION_STATE APP_STATE;

extern _FLAGS FLAGS;
extern _MODEM_TASKS mTask;
extern _SMS_TASKS SMS_TASKS;
extern  RXSMS_CMDS RxdSMSCmd;
extern _SYSTEM_TASKS SYSTEM_TASK;
extern _MODBUS_STATE MODBUS_STATE;
extern _MODEM_STATUS MODEM_STATUS;

BYTE pTaskRxFifo[TASK_FIFO_SIZE];
_MODEM_TASKS pModemTaskRxFifo[TASK_FIFO_SIZE];

static volatile BYTE *TaskHeadPtr = pTaskRxFifo, *TaskTailPtr = pTaskRxFifo;
static volatile _MODEM_TASKS *MTaskHeadPtr = pModemTaskRxFifo, *MTaskTailPtr = pModemTaskRxFifo;


UINT8 INTProcessPending = 0;


void Application_Tasks(void)
{
    switch(APP_STATE)
    {
        case APP_INIT:
            InitSYSTasks();
            ReadPrevTime();

            #ifdef CLT_SYS_PARS_TEST
            InsertSYSTask(CLT_SYS_READ_TASK);
            DelayMs(9000);
            #endif

            APP_STATE = APP_INIT_MODEM;
            break;

        case APP_INIT_MODEM:
            #ifdef EN_SLEEEP
            APP_STATE = APP_STANDBY;
            #else
            if(MODEM_STATUS == MODEM_INITIALIZED)
            {
                //need to check correct place or not
                if(FLAGS.SysIsRestartedBySMS)
                {
                    FLAGS.SysIsRestartedBySMS = 0;
                    InsertSYSTask(SEND_RESTART_ACK_TASK);
                }
                APP_STATE = APP_STANDBY;
            }
            #endif
            break;
            
        case APP_GET_CLTSYS_PARS:
            MODBUS_STATE = MODBUS_RMC_SBR_SYS_PAR;
            APP_STATE = APP_STANDBY;
            break;
            
        case APP_GET_ALARMS:
            APP_STATE = APP_STANDBY;
            break;

        case APP_GET_EM_PARS:
            APP_STATE = APP_STANDBY;
            break;

        case APP_PLC_RTC_TASK:
            MODBUS_STATE = MDB_SET_PLC_RTC;
            APP_STATE = APP_STANDBY;
            break;
        
        case APP_SET_TIMERS:
            MODBUS_STATE = MDB_SET_TIMERS;
            APP_STATE = APP_STANDBY;
            break;
            
        case APP_RESTART_PLC:
            MODBUS_STATE = MDB_RESET_PLC;
            APP_STATE = APP_STANDBY;
            break;
        
        case APP_SET_SBR_MAINTENANCE:
            MODBUS_STATE = MDB_SBR_MAINTENANCE;   
            APP_STATE = APP_STANDBY;
            break;    
            
        case APP_PUT_RST_SMS_ACK:
            SMS_TASKS.Parameters[0] = 0xFF;
            SMS_TASKS.SMS_TASK_STATE = RESTART_RMCU;
            InsertSMSTask(SMS_TASKS);
            mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
            InsertMODEMTask(mTask);
            APP_STATE = APP_STANDBY;
            break;

        case APP_PUT_DEV_INFO:
            mTask.MODEM_TASK_STATE = MODEM_SEND_DEV_INFO_TASK;
            InsertMODEMTask(mTask);
            APP_STATE = APP_STANDBY;
            break;
            
        case APP_PUT_CLT_SYS_PARS:
            Construct_SBR_Summery_ParamsPacket();
            Construct_SBR_Status_ParamsPacket();
            Construct_EMP_ParamsPacket();
            Construct_SBR_Settings_ParamsPacket();
            Construct_SBR_Alarms_Packet();
            mTask.MODEM_TASK_STATE = MODEM_SEND_CLT_SUMMERY_TASK;
            InsertMODEMTask(mTask);
            APP_STATE = APP_STANDBY;
            break;
            
        case APP_FRAME_BYTE_STREAM:
            MODBUS_STATE = MODBUS_FRAME_BYTE_STREAM;
            APP_STATE = APP_STANDBY;
            break;

           #ifdef EN_SLEEP
        case APP_SYS_IDLE:
            break;

        case APP_READY_FOR_SLEEP:
            InsertSYSTask(SYSTEM_SLEEP_TASK);
            APP_STATE = APP_STANDBY;
            break;

        case APP_SLEEP:
            SetALMTime();
            // Device will enter SLEEP mode
            OSCCONSET = 0x10; 
            PowerSaveSleep();
            APP_STATE = APP_STANDBY;
            break;
            #endif

        case APP_STANDBY:

            if(((STASK = GetSYSTask())!= 0))
            {
                if(STASK == DEV_INFO_UPLOAD_TASK)
                {
                    APP_STATE = APP_PUT_DEV_INFO;
                }

                else if(STASK == CLT_SYS_UPLOAD_TASK)
                {
                    APP_STATE = APP_PUT_CLT_SYS_PARS;
                }

                else if(STASK == CLT_SYS_READ_TASK)
                {
                    APP_STATE = APP_GET_CLTSYS_PARS;
                }
                 
                else if(STASK == GET_PLC_SETTINGS_TASK)
                {
                    FLAGS.GetPLCSettingsSMScmd = 1;
                    APP_STATE = APP_GET_CLTSYS_PARS;
                }

                else if(STASK == SEND_RESTART_ACK_TASK)
                {
                    APP_STATE = APP_PUT_RST_SMS_ACK;
                }

                /*else if(STASK == SET_TIMER1_TASK)
                {
                    APP_STATE = APP_SET_TIMER1;
                }

                else if(STASK == SET_TIMER2_TASK)
                {
                    APP_STATE = APP_SET_TIMER2;
                }

                else if(STASK == SET_TIMER3_TASK)
                {
                    APP_STATE = APP_SET_TIMER3;
                }
                
                else if(STASK == SET_TIMER4_TASK)
                {
                    APP_STATE = APP_SET_TIMER4;
                }*/

                else if(STASK == SET_TIMERS_TASK)
                {
                    APP_STATE = APP_SET_TIMERS;
                }

                /*else if(STASK == SET_P1CURRENT_TASK)
                {
                    APP_STATE = APP_SET_P1CURRENT;
                }

                else if(STASK == SET_P2CURRENT_TASK)
                {
                    APP_STATE = APP_SET_P2CURRENT;
                }

                else if(STASK == SET_P3CURRENT_TASK)
                {
                    APP_STATE = APP_SET_P3CURRENT;
                }*/



               else if(STASK == RESTART_PLC_TASK)
                {
                    APP_STATE = APP_RESTART_PLC;
                }

                
                else if(STASK == SET_SBR_MAINTENANCE_TASK)
                {
                    APP_STATE = APP_SET_SBR_MAINTENANCE;
                }
                
                else if(STASK == FRAME_BYTE_STREAM_TASK)
                {
                    APP_STATE = APP_FRAME_BYTE_STREAM;
                }
                else if(STASK == PLC_RTC_TASK)
                {
                    APP_STATE = APP_PLC_RTC_TASK;
                }


            }
            break;
            
            #ifdef EN_SLEEP
            else if(STASK == SYS_NO_TASK)
            {
                if(MODEM_STATE == MODEM_WAIT)
                {
                    if(AppStatus._FLAGS.CHPL_MM_DONE && AppStatus._FLAGS.GWMM_DONE && AppStatus._FLAGS.IMGCAP_DONE
                            &&AppStatus._FLAGS.IMPMM_DONE && AppStatus._FLAGS.NPKMM_DONE)
                    APP_STATE = APP_SYS_IDLE;
                }
                    
                Nop();

            }
            #endif

            case  APP_SYS_IDLE:
            break;
            case  APP_READY_FOR_SLEEP:
            break;
            case  APP_SLEEP: 
            break;
            case  APP_GET_PLC_SETTINGS:
            break;
            case  APP_PUT_ALARMS:
            break;

            break;
    }

}



void InsertSYSTask(_SYS_TASKS SysTask)
{
    *TaskHeadPtr++ = SysTask;

    if(TaskHeadPtr >= pTaskRxFifo + sizeof(pTaskRxFifo))
        TaskHeadPtr = pTaskRxFifo;
}

_SYS_TASKS GetSYSTask(void)
{
    _SYS_TASKS SysTask;

    if(TaskHeadPtr != TaskTailPtr)
    {
        SysTask = *TaskTailPtr++;

        if(TaskTailPtr >= pTaskRxFifo + sizeof(pTaskRxFifo))
            TaskTailPtr = pTaskRxFifo;
    }
    else
    {
        SysTask = SYS_NO_TASK;
    }

    return SysTask;
}

void InitSYSTasks(void)
{
    TaskHeadPtr = pTaskRxFifo;
    TaskTailPtr = pTaskRxFifo;
}


void InsertMODEMTask(_MODEM_TASKS ModemTask)
{
    *MTaskHeadPtr++ = ModemTask;

    if(MTaskHeadPtr >= pModemTaskRxFifo + sizeof(pModemTaskRxFifo))
        MTaskHeadPtr = pModemTaskRxFifo;
}

_MODEM_TASKS GetMODEMTask(void)
{
    _MODEM_TASKS ModemTask;

    if(MTaskHeadPtr != MTaskTailPtr)
    {
        ModemTask = *MTaskTailPtr++;

        if(MTaskTailPtr >= pModemTaskRxFifo + sizeof(pModemTaskRxFifo))
            MTaskTailPtr = pModemTaskRxFifo;
    }
    else
        ModemTask.MODEM_TASK_STATE = MODEM_NO_TASK;

    return ModemTask;
}

void InitModemTasks(void)
{
    MTaskHeadPtr = pModemTaskRxFifo;
    MTaskTailPtr = pModemTaskRxFifo;
}


void __attribute__((__interrupt__, auto_psv)) _INT0Interrupt(void)
{
    if(INTProcessPending == 0)
    {
        INTProcessPending = 1;
        //Read CLT Sysytem parameters
        InsertSYSTask(CLT_SYS_READ_TASK);
    }
    IFS0bits.INT0IF = 0;
}
