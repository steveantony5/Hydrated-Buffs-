
/***************************************************************************//**
 * @Author	Sorabh Gandhi
 * @Date	02/13/2019
 * @file main.c
 * @brief Bluetooth Thermometer
 * This Thermometer allows to measure temperature using the temperature sensor SI7021,
 * from a mobile app.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 *****************************************************************************
 *	@Reference	https://www.silabs.com/community/mcu/32-bit/forum.topic.html/prs_and_le_periphera-2AhC - Bluetooth SDK example - soc-thermometer
 *				https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf - Temperature data sheet
 *				https://www.silabs.com/documents/login/quick-start-guides/qsg139-getting-started-with-bluetooth.pdf - GATT Documentation
 *				https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.health_thermometer.xml
 *				https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.temperature_measurement.xml
 *				https://www.silabs.com/documents/login/reference-manuals/bluetooth-api-reference.pdf - API Refernce Manual
 *
 *	@Configuration Method - PERIOD, LED_ON_TIME and ENERGY_MODE are defined in main.h
 ******************************************************************************/

//Own Header
#include "main.h"

/*******************************************************************************
 *  				PROTOTYPES OF LOCAL FUNCTIONS
 ******************************************************************************/
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];
extern bool gecko_update(struct gecko_cmd_packet *evt);
void gecko_ecen5823_update(struct gecko_cmd_packet *evt);

/*******************************************************************************
 *  				STRUCTURES
 ******************************************************************************/
// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};

/*******************************************************************************
 *  				GLOBAL VARIABLES
 ******************************************************************************/
/* Flag for indicating DFU Reset must be performed */
uint8_t boot_to_dfu;

volatile uint32_t water_level_state = 0;
volatile uint32_t calibirate_capacitance_state = 0;
volatile uint32_t minimum_water_level = 0;
volatile uint32_t maximum_water_level = DEFAULT_MAX_WATER_LEVEL;
volatile uint32_t current_water_level = 0;
volatile uint32_t water_consumed = 0;
volatile uint8_t accel_state;

/* https://forum.mikroe.com/viewtopic.php?f=164&t=67801 */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void publish_water_level(int water_level) {
	uint8_t databuffer[5];
	uint8_t *p = databuffer;
	uint8_t flag = 0x00;

	itoa(water_level, databuffer, 10);   // here 10 means decimal
	LOG_INFO("Decimal value for water level = %s\n", databuffer);

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_water_consumed_value, 5, databuffer));
}
/*******************************************************************************
 *  				MAIN FUNCTION
 ******************************************************************************/
int main(void)
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  logInit();

  // Initialize stack
  gecko_init(&config);

  cmu_init();

  //Initialize LETIMER
  timer_Init();

  gpioInit();

  enable_button_interrupts();

  /* Infinite loop */
  while (1) {

	  /*event pointer for handling events*/
	  struct gecko_cmd_packet *event;

	  /*Check of local event*/
	  event = gecko_wait_event();

	  gecko_ecen5823_update(event);
  }
}


void gecko_ecen5823_update(struct gecko_cmd_packet *evt)
{
	gecko_update(evt);

	switch BGLIB_MSG_ID(evt->header) {

		case gecko_evt_system_boot_id:

			//Set the advertisement interval
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, MINIMUM_ADVT_INTERVAL, MAXIMUM_ADVT_INTERVAL, 0, 0));

			//Set the advertisement mode as discoverable
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));

#if 0
			BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(1));
			//Initially set the TX power as 0
			gecko_cmd_system_set_tx_power(0);
			BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(0));
#endif
			//Disable the timer Initially
			LETIMER_Enable(LETIMER0, false);

			//Disable the I2C bus Initially
			I2C_Enable(I2C0, true);
			break;

		case gecko_evt_le_connection_closed_id:
			if (boot_to_dfu) {
				/* Enter to DFU OTA mode */
				gecko_cmd_system_reset(2);
			} else {
				//Disable the timer
				LETIMER_Enable(LETIMER0, false);

				//Disable the I2C bus
				I2C_Enable(I2C0, false);

				gecko_cmd_hardware_set_soft_timer(0, 0, 0);

				/* Restart advertising after client has disconnected  */
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
			}
			break;

		case gecko_evt_system_external_signal_id:

			//Call the I2C state machine to process local events and fetch the I2C value
			if(EXT_SIGNAL_PB0_BUTTON == 1) {
				calibirate_capacitance_state += 1;

//				water_level_state += 1;
				EXT_SIGNAL_PB0_BUTTON = 0;
				gecko_cmd_hardware_set_soft_timer(ACCEL_DELAY, 0, 1);

			}
			else {
				state_machine(evt->data.evt_system_external_signal.extsignals);
			}

			break;

		case gecko_evt_hardware_soft_timer_id:

			if(calibirate_capacitance_state == 1) {
//				accel_state = accel_read();
				int cap_raw_value = cap_read();
				minimum_water_level = cap_raw_value;
				maximum_water_level = cap_raw_value + 1500;
				LOG_INFO("Minimum water value = %d\n", minimum_water_level);
				LOG_INFO("Maximum water value = %d\n", maximum_water_level);

				LOG_INFO("Calibiration state cap value = %d\n", cap_raw_value);
				int ml = map(cap_raw_value, minimum_water_level, maximum_water_level, 0 , 700);

				if(ml < 0) {
					ml = 0;
				}


			}
			else {
				accel_state = accel_read();

				LOG_INFO("Accel state = %d\n", accel_state);
				water_level_state = 0;
				uint32_t cap_raw_value = cap_read();
				LOG_INFO("Before capacitor compare = %d\n", cap_raw_value);
				if(cap_raw_value > maximum_water_level) {
					maximum_water_level  = cap_raw_value;
				}

				LOG_INFO("After capacitor compare = %d\n", cap_raw_value);
				int ml = map(cap_raw_value, minimum_water_level, maximum_water_level, 0 , 700);

				LOG_INFO("ML Value = %d\n",ml);
				if(ml < 0) {
					ml = 0;
				}

				if(accel_state == DRINKING_STATE) {
					water_consumed += (current_water_level - ml);
				}

				else if(accel_state == IDLE_STATE) {
					current_water_level = ml;
				}

				else if(accel_state == POURING_STATE) {
					current_water_level = ml;
				}

				publish_water_level(ml);
			}
			NVIC_EnableIRQ(GPIO_ODD_IRQn);
			NVIC_EnableIRQ(GPIO_EVEN_IRQn);

			break;
		case gecko_evt_le_connection_opened_id:


			//Peiod of connection after client has connected
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(evt->data.evt_le_connection_opened.connection, MAXIMUM_CONNECTION_INTERVAL, MINIMUM_CONNECTION_INTERVAL, SLAVE_LATENCY, TIMEOUT));

			//Store the connection handle in the global variable
			connection_handle = evt->data.evt_le_connection_opened.connection;

			//Enable the timer
			LETIMER_Enable(LETIMER0, true);

			//Enable the I2C bus
			I2C_Enable(I2C0, true);

//			gecko_cmd_hardware_set_soft_timer(98304, 0, 0);
			break;

		case gecko_evt_le_connection_rssi_id:

			//read the RSSI value stored in the evt_le_connection_rssi structure returned after calling gecko_cmd_le_connection_get_rssi
			rssi_value = evt->data.evt_le_connection_rssi.rssi;
#if 0
			tx_power = 0;

			if (rssi_value > -35) {
				tx_power = -26;
			}
			else if ((rssi_value > -45) && (rssi_value <= -35)) {
				tx_power = -20;
			}
			else if ((rssi_value > -55) && (rssi_value <= -45)) {
				tx_power = -15;
			}
			else if ((rssi_value > -65) && (rssi_value <= -55)) {
				tx_power = -5;
			}
			else if ((rssi_value > -75) && (rssi_value <= -65)) {
				tx_power = 0;
			}
			else if ((rssi_value > -85) && (rssi_value <= -75)) {
				tx_power = 5;
			}
			else if ((rssi_value <= -85)) {
				tx_power = 8;
			}
			else {
				//tx_power = 0;
			}
			BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(1));
			//Set the TX power according to the RSSI range
			gecko_cmd_system_set_tx_power(tx_power * 10);
			BTSTACK_CHECK_RESPONSE(gecko_cmd_system_halt(0));
#endif
			break;
	}
}
