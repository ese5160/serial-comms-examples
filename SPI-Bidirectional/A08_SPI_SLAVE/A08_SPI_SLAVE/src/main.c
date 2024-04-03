/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>

uint16_t dummy_length = 1;
uint16_t buffer_length = 2;
uint16_t unused_dummy = 0x55;
uint8_t rx_buffer[2];
uint8_t tx_buffer[2] = {0x10,0x20};
uint8_t dummy_buffer;
uint8_t dummy_rx_buffer;
bool data_status = 0;

struct spi_module spi_slave_instance;

// function prototypes
void configure_spi_slave(void);

int main (void)
{
	system_init();

	/* Insert application code here, after the board has been initialized. */
	configure_spi_slave();
	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		
		/*read data from master*/
		spi_read_buffer_wait(&spi_slave_instance,rx_buffer,buffer_length,unused_dummy);
		 
		/* validate the received data */
		if((rx_buffer[0] == 0x25) && (rx_buffer[1] == 0x56)){
			
			/*set data status */
			data_status = 1;
			
			/* wait for dummy character */
			spi_read_buffer_wait(&spi_slave_instance,&dummy_buffer,dummy_length,unused_dummy);
			
			/* if dummy received*/
			if((dummy_buffer == 0xDD) && (data_status)){
				
				/*write to master*/
				spi_write_buffer_wait(&spi_slave_instance,&tx_buffer,buffer_length);
			}
		 }
		 else {
			 data_status = 0;
		 }
	}
}

void configure_spi_slave(){
	
	struct spi_config config_spi_slave;
	
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_slave);
	
	/*set mode of operation as slave*/ 
	config_spi_slave.mode = SPI_MODE_SLAVE;
	/*configure the pins */
	/*In slave operation: DI is MOSI, DO is MISO.*/
	config_spi_slave.mux_setting = SPI_SIGNAL_MUX_SETTING_O;
	config_spi_slave.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;//miso
	config_spi_slave.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;// ss
	config_spi_slave.pinmux_pad2 = PINMUX_PA18C_SERCOM1_PAD2;//mosi
	config_spi_slave.pinmux_pad3 = PINMUX_PA19C_SERCOM1_PAD3;//sck
	
	/* enter custom settings*/
	config_spi_slave.mode_specific.master.baudrate = 500000;  // 0.5 MHZ clock.
	config_spi_slave.transfer_mode = SPI_TRANSFER_MODE_1;  // works with both mode 0 and mode 1
	config_spi_slave.select_slave_low_detect_enable = false;
	
	spi_init(&spi_slave_instance, SERCOM1, &config_spi_slave);
	spi_enable(&spi_slave_instance);
	
}