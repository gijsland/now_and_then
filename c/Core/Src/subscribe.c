/*
 * Generated by can_node.py. Do not manually edit this file.
 */

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "task/task_can_bus.h"
#include <shared_can_defs.h>
#include <string.h>
#include "util.h"
#include "subscribe.h"

CAN_MSG_VCU_pedals_T m_CAN_MSG_VCU_pedals;
uint32_t VCU_pedals_rx_time;

CAN_MSG_VCU_switches_T m_CAN_MSG_VCU_switches;
uint32_t VCU_switches_rx_time;

CAN_MSG_charger_control_T m_CAN_MSG_charger_control;
uint32_t charger_control_rx_time;

void eat_new_data(CAN_rx_t* recv)
{
	void* dest = 0;
	uint32_t* time_address = 0;

	if (recv->header.ExtId == CAN_ID_VCU_pedals)
	{
		dest = &m_CAN_MSG_VCU_pedals;
		time_address = &VCU_pedals_rx_time;
	}
	else if (recv->header.ExtId == CAN_ID_VCU_switches)
	{
		dest = &m_CAN_MSG_VCU_switches;
		time_address = &VCU_switches_rx_time;
	}
	else if (recv->header.ExtId == CAN_ID_charger_control)
	{
		dest = &m_CAN_MSG_charger_control;
		time_address = &charger_control_rx_time;
	}

	if (dest != 0) {
		memcpy(dest, &(recv->data), recv->header.DLC);
		*time_address = get_timestamp();
	}
}
