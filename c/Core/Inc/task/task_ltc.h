//
// Created by gijsl on 3/15/2021.
//

#ifndef NOW_AND_THEN_LTC_TASK_H
#define NOW_AND_THEN_LTC_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Define the number of battery segments (number of LTC6811 chips) here.
 * The main task will send instructions once, it is up to task_ltc to handle
 * repeating SPI commands the appropriate number of times.
 */
#define num_segments 8

void task_ltc_setup(void);
void send_start_balance_cmd(void);
void send_stop_balance_cmd(void);
void wake_ltc_routine(void);
void write_comm_routine(void);
void write_config_routine(void);
void update_mux_routine(void);
void read_cvr_routine(void);
void read_temp_routine(void);
void read_all_temp_routine();

//void start_task_ltc(void *argument);
[[noreturn]] void start_task_ltc(void *argument);

typedef struct {
    void * placeholder;
} ltc_spi_tx_request_t;

extern QueueHandle_t ltc_spi_tx_Q;

typedef struct {
    void * placeholder;
} ltc_spi_rx_t;

extern QueueHandle_t ltc_spi_rx_Q;

extern QueueHandle_t balance_cmd_Q;

typedef struct {
    bool do_balance;
} ltc_balance_cmd_t;

#ifdef __cplusplus
}
#endif

#endif //NOW_AND_THEN_LTC_TASK_H
