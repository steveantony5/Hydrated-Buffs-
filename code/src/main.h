/*
 * main.h
 *
 *  Created on: 30-Jan-2019
 *      Author: Sorabh
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/*Own Header*/
#include "my_sleep.h"
#include "cmu.h"
#include "timer.h"
#include "log.h"
#include "scheduler.h"
#include "i2c_sensor.h"
#include "gecko_ble_errors.h"
#include "gpio.h"
#include "i2c_config.h"

#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"

/*******************************************************************************
*				DEFINITIONS AND MACROS
*******************************************************************************/
/* Define the energy mode using this macro
 * --> For EM0, define ENERGY_MODE as 0
 * --> For EM1, define ENERGY_MODE as 1
 * --> For EM2, define ENERGY_MODE as 2
 * --> For EM3, define ENERGY_MODE as 3
 */
#define ENERGY_MODE 0

/* Total time period for the LED */
#define PERIOD (3)

/* ON time period for the LED */
#define LED_ON_TIME (0.175)

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif

#define MINIMUM_ADVT_INTERVAL 160
#define MAXIMUM_ADVT_INTERVAL 160

#define MINIMUM_CONNECTION_INTERVAL 60
#define MAXIMUM_CONNECTION_INTERVAL 60
#define SLAVE_LATENCY 3
#define TIMEOUT 400


/*******************************************************************************
 *  				EXPORTED VARIABLES
 ******************************************************************************/
int8 rssi_value;
int16 tx_power;
uint8 connection_handle;

#endif /* SRC_MAIN_H_ */
