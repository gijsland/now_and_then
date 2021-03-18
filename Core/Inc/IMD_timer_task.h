//
// Created by gijsl on 3/17/2021.
//

#ifndef NOW_AND_THEN_IMD_TIMER_TASK_H
#define NOW_AND_THEN_IMD_TIMER_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

typedef struct {
    uint32_t frequency;
    uint32_t duty_cycle;
} IMD_message;

extern QueueHandle_t IMD_Q;

void create_IMD_queue(void);

#endif //NOW_AND_THEN_IMD_TIMER_TASK_H