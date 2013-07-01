#include "XBeeMACLayer.h"
#include "PicoIPv6Connection.h"


XBeeMACLayer macLayer; 					// XBee MAC Layer
PicoIPv6Connection* picoIPv6Connection;	// PicoIPv6 Connection
u8_t picoip_buffer[UIP_BUFSIZE]; 		// Defines the picoIPv6 packet buffer.

// Definitions for the UDP sending Loop
#define UDP_SEND_TIME 10000 // UDP loop period (miliseconds)
struct timer udp_msg_timer; // UDP loop timer

void setup() {

	Serial.begin(9600);

	// Start the MACLayer and PicoIPv6 Connection:
	// 	1) Initialization of the MAC Layer.
	if ( !macLayer.init() ) {
		 while (1){}; // Cannot initialize MAC Layer and will not continue.
	}

	// 2) Initialization of the picoIPv6 Connection.
	picoIPv6Connection = new PicoIPv6Connection(&macLayer, picoip_buffer); // Definition of an object of the class PicoIPv6Connection


	// 3) Set up of the timer for the UDP Message Loop.
	timer_set(&udp_msg_timer, UDP_SEND_TIME);

}


// Definitions for the UDP Message to be sent
const u16_t udp_source_port			= 8765; // UDP Source Port Number.
const u16_t udp_destination_port	= 8765; // UDP Destination Port Number.
static uip_ipaddr_t remote_client_ipaddr = {0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; // Remote client IPv6 Address. (bbbb::1)

// Definition of UDP message payload buffer and messages counter.
char udp_msg_buf[UIP_UDP_PAYLOAD_MAX_BUFSIZE];	// UDP payload buffer.
static uint16_t udp_msg_counter = 0; 			// UDP number of messages sent counter.

void loop() {

	if (picoIPv6Connection->isConnected()){
		// There is a connection at picoIPv6 level.
		if (timer_expired(&udp_msg_timer)){
			// UDP timer has expired. We send an UDP Message.
			//	1) Create UDP Message to be sent.
			++udp_msg_counter;
			sprintf(udp_msg_buf, "UDP Message Nr.%hu\n", udp_msg_counter); // [ %hu h:short u:unsigned ]

			//	2) Send UDP Message.
			picoIPv6Connection->udp_send(&remote_client_ipaddr, udp_source_port, udp_destination_port, udp_msg_buf, strlen(udp_msg_buf));

			//	3) Restart UDP timer.
			timer_restart(&udp_msg_timer);

		}//else{} // UDP timer has NOT expired.
	}
	else{// There is NOT a connection at picoIPv6 level.
		picoIPv6Connection->connect(); // We initialize a picoIPv6 connection.
		delay(200); // Add a delay time to establish a picoIPv6 connection.
	}

}


/* This function is automatically invoked when we call PicoIPv6Connection->receive() and there is new UDP Data.
 * 		* udp_data:	contains new UDP datagram payload.
 * 		* length: 	contains new UDP datagram payload lenght.
 *
 * 			[	At lower level it is actually called from PicoIPv6State.udp_input() ]
 * 			[	only when the PicoIPv6State subclass is ConnectedState				]
 * */
void udp_input_callback(uint8_t* udp_data, uint16_t length) {}
