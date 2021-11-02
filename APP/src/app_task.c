#include "app_data.h"
#include "app_device.h"
#include "app_task.h"

void app_createMainTask()
{
    xTaskCreate( vTask2, "Task 2", 1000, NULL, 1, NULL );
}
void app_createDriverTask();
void app_createCommunicationTask();

/**
 * 主任务
 */
void app_mainTask(void * param)
{
    Device * pdev = (Device *)param;

    switch(pdev->state)
    {
        case IDLE:
        {

        }   break;

        case UNCONFIG:
        {

        }   break;

        case CONFIGED:
        {

        }   break;

        case GATEIAP:
        {

        }   break;

        case BTLIAP:
        {

        }   break;

        case RESTART:
        {

        }   break;

        case RESTORE:
        {

        }   break;

        case TIPS:
        {

        }   break;

        case TEST:
        {

        }   break;

        default:
        {

        }  break;
    }
}

void app_driverTask()
{
    
}

void app_CommunicationTask()
{

}