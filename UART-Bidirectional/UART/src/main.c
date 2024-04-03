/**
Author: Tarush Sharma 

Description:This code is to initiate bi-direction Usart communication. 
**/
#include <asf.h>
#include <sercom.h>

// data buffers for UART
uint8_t tx_buffer = {0x10,0x22,0x33};
uint16_t rx_buffer[3]; 
// usart master module
struct usart_module usart_master_module;

// function definition
void usart_setup(void);

int main (void)
{
	system_init();

	/* Insert application code here, after the board has been initialized. */
	usart_setup();

	while (1) {
		// define error codes to check the success of tx and rx 
		status_code_genare_t status_tx_check = STATUS_OK;
		status_code_genare_t status_rx_check = STATUS_OK;		
		
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			// transmit usart message 
			status_tx_check = usart_write_wait(&usart_master_module,tx_buffer);
			if(status_tx_check != STATUS_OK){
				 //enter your debugging code here
				;
			}
		}
		delay_ms(100);
		// poll the receiver continuously
		status_rx_check = usart_read_wait(&usart_master_module,&rx_buffer[0]);
		if(status_rx_check != STATUS_OK){
			// enter your debugging code here
			;
		}
		// if data is valid 
		if( rx_buffer[0] == 0x10 ){
			// switch on the led
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		}
		else {
			// switch off the led
		port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}

void usart_setup(void){
	status_code_genare_t status_check = STATUS_OK;
	// setup the usart
	struct usart_config usart_setup;
	// set default values
	usart_get_config_defaults(&usart_setup);
	//configure pins and pinmux for sercom 4
	usart_setup.mux_setting = USART_RX_3_TX_2_XCK_3;
	usart_setup.pinmux_pad2 = PINMUX_PB10D_SERCOM4_PAD2;
	usart_setup.pinmux_pad3 = PINMUX_PB11D_SERCOM4_PAD3;
	// write the configuration to the master module
	status_check = usart_init(&usart_master_module, SERCOM4 ,&usart_setup);
	if(status_check != STATUS_OK){
		// put your debugging command here 
		;
	}
	//enable the usart
	usart_enable(&usart_master_module);
	
}