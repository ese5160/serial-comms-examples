/**
Author: Tarush Sharma 

Description: This code is used initiate full duplex communication using Serial Peripheral Interface
 */
#include <asf.h>

// data buffers for spi 
uint16_t buffer_length = 2;
uint16_t dummy_char = 0xDD;
const uint8_t tx_buffer[2] = {0x25, 0x56};
uint8_t rx_buffer[2];
bool take_slave = 0;
bool release_slave = 1;
struct spi_module spi_master_instance;
struct spi_slave_inst slave;

// function prototypes
void configure_spi_master(void);


int main (void)
{
	system_init();

	/* Insert application code here, after the board has been initialized. */
	configure_spi_master();
	bool i = 0;
	bool j = 0;
	/* Yes, so turn LED on. */
	port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		status_code_genare_t send = STATUS_OK;
		status_code_genare_t ss = STATUS_OK;
		/*If button is pressed */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/*send SPI data*/
			/*pull the SS line low*/
			ss = spi_select_slave(&spi_master_instance,&slave,release_slave);
			if(send != STATUS_OK){
				j=1;
			}
			/*write data on mosi line*/
			send = spi_write_buffer_wait(&spi_master_instance, tx_buffer, buffer_length);
			if(send != STATUS_OK){
				i=1;
			}
			/*pull the SS line high*/
			ss = spi_select_slave(&spi_master_instance,&slave,take_slave);
			if(send != STATUS_OK){
				j=1;
			}	

			/* initiate read*/
			/*pull the SS line low*/
			spi_select_slave(&spi_master_instance,&slave,release_slave);
			
			/* send the dummy character for reading*/ 			
			spi_read_buffer_wait(&spi_master_instance,rx_buffer,buffer_length,dummy_char);
			
			//delay_ms(5);
			/*pull the SS line high*/
			ss = spi_select_slave(&spi_master_instance,&slave,take_slave);
			
			/* validate the received data */
			if((rx_buffer[0] == 0x10) && (rx_buffer[1] == 0x20)){

				/*switch on the led*/
				port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			}
			else{
				
				/*switch off led*/
				port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
			}
			
			//delay_ms(10);
		
		} 
		else {
			/* switch off led*/
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}

void configure_spi_master(){
	
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	
	/*get default slave configs*/
	/*slave addressing is disabled by default*/
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = PIN_PA02;
	spi_attach_slave(&slave, &slave_dev_config);
	
	/*configure the pins */
	/*Master config -> DI is MISO, DO is MOSI.*/
	config_spi_master.mux_setting = SPI_SIGNAL_MUX_SETTING_E; 
	config_spi_master.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;//miso
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;// Pin A02 is used as ss
	config_spi_master.pinmux_pad2 = PINMUX_PA18C_SERCOM1_PAD2;//mosi
	config_spi_master.pinmux_pad3 = PINMUX_PA19C_SERCOM1_PAD3;//sck
	
	/* enter custom settings*/
	config_spi_master.mode_specific.master.baudrate = 500000;  // 0.5 MHZ clock.
	config_spi_master.transfer_mode = SPI_TRANSFER_MODE_1;  // works with both mode 0 and mode 1
	config_spi_master.select_slave_low_detect_enable = false;
	
	spi_init(&spi_master_instance, SERCOM1, &config_spi_master);
	spi_enable(&spi_master_instance);
	
}
