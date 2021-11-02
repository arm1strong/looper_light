/**********************************************************
 * 该文件用于实现跳传算法
 * 
 * 
 * 
 * 
 * 
 *********************************************************/
#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "stdint.h"
#include "app_protocol.h"
#include "app_device.h"
#include "app_data.h"

typedef enum
{
	COMMU_IDEL,		  //空闲态 无数据交互
	COMMU_ALIGNMENT,  //时间片对齐状态 当收到上游设备广播后进入该状态 等待时间片对齐后执行亮灯和下一节点的跳传 目的地是各节点亮灯间隔保持一致
	COMMU_BROADCAST,  //广播模式 向下游设备广播 收到下游设备广播后立即停止广播
}COMMU_STATE;


typedef struct
{
	COMMU_STATE state; //运行状态
	uint8_t sn;		   //当前时间片

	/*对下游单个设备的广播最大次数 信号不好可以增加该值*/
	uint8_t max_broadcast_times;

	/*广播给下游最大设备数 在信号覆盖范围内 可通过增加此值提高广播容灾性 正常情况下ID只给ID+1广播 若ID+1损坏则向ID+2广播 
	最大广播到ID+max_broadcast_dev_cnts*/
	uint8_t max_broadcast_dev_cnts;

	/*广播周期 信号不好可以提高max_broadcast_times 并降低max_broadcast_period减少延迟*/
	uint16_t max_broadcast_period;

	uint16_t tick; 		//用于计数 每次切换状态都应复位此值为0

	/*********************************
	 * self_id < start_id	: 只跳传不应用灯光
	 * self_id >= start_id && self_id < end_id : 跳传并应用灯光
	 * self_id == end_id	: 不跳传应用灯光
	 * self_id > end_id		: 不跳传不应用灯光
	 *********************************/
	uint16_t self_id;	//自身ID
	uint16_t start_id;	//起始ID
	uint16_t end_id;	//起始ID
	uint16_t id_index;	//当前ID索引 用于标记下游控制器的ID

	uint8_t msg[COMMU_MSG_SIZE]; //封装为消息而非明确的8通道灯光亮度 目的是使其跳传具备一定的通用性
}Communication;

COMMU_STATE app_pollCommunicationEvents(Communication * pcommu);

#endif
