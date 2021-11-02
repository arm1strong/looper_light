/*****************************************************************
 *  说明:任务处理 工程最高层 分析从此着手
 * 
 *  日期:2021-8-24 13:54:22
 *****************************************************************/
#ifndef __TASK_H
#define __TASK_H

#include "stdint.h"
#include "app_protocol.h"
#include "app_device.h"

/**
 * 主任务 
 */
void app_mainTask(void * param);
void app_communicationTask(void *argument);
void app_driverTask(void *argument);

void app_createAllTask(void);

#endif
