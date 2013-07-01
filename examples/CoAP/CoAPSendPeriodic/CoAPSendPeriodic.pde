#include "XBeeMACLayer.h"
#include "PicoIPv6Connection.h"
#include "CoAPEngine.h"


XBeeMACLayer macLayer; 					// XBee MAC Layer
PicoIPv6Connection picoIPv6Connection;	// PicoIPv6 Connection
u8_t picoip_buffer[UIP_BUFSIZE]; 		// Defines the picoIPv6 packet buffer.
CoAPEngine coap;						// CoAPEngine-alpha. Sends CoAP PUT Non-Confirmable messages with URI, Query, optional second Query, and payload.

// Definitions for the Data/CoAP Sending Loop
#define DATA_SEND_PERIOD 5000 // loop period (miliseconds)
struct timer data_send_timer; //loop timer




void setup() {

	/*Serial.begin(9600);*/ // Disabled for more RAM

	// Start the MACLayer and PicoIPv6 Connection:
	// 	1) Initialization of the MAC Layer.
	if ( !macLayer.init() ) {
		 while (1){}; // Cannot initialize MAC Layer and will not continue.
	}

	// 2) Initialization of the picoIPv6 Connection.
	picoIPv6Connection.init(&macLayer, picoip_buffer); // Definition of an object of the class PicoIPv6Connection

	// 3) Initialize the CoAP Engine with a picoIPv6 connection.
	coap.setPicoIPv6Connection(picoIPv6Connection);

	// 4) Set up of the timer for the Data/CoAP Sending Loop.
	timer_set(&data_send_timer, DATA_SEND_PERIOD);

}


// Definitions for the CoAP Message to be sent
static uip_ipaddr_t coap_server_addr 	= {0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; // Remote server IPv6 Address. Ex.: [bbbb::3]

String coap_uri = "uri_test"; 			// CoAP Option: URI
String coap_query = "apikey=fd205ed589";// CoAP Option: Query. Emoncms: "apikey=<your_api_key>"
String coap_query_opt = "node=1";		// CoAP Option: Query (Optional)
String coap_payload_string = " ";		// CoAP Payload.

static uint16_t msg_counter = 0; // number of messages sent counter.

void loop() {

	if (picoIPv6Connection.isConnected()){
		// There is a connection at picoIPv6 level.
		if (timer_expired(&data_send_timer)){
			// Send timer has expired. We send an CoAP Message.
			//	1) Obtain the data to be sent and set the proper CoAP URI.
			coap_uri = "msg_counter";
			++msg_counter;
			coap_payload_string = String(msg_counter);

			//	2) Send the CoAP Message.
			coap.sendCoAPpacket(coap_server_addr, coap_uri , coap_query , coap_query_opt , coap_payload_string);


			// Repeat steps 1 and 2 for any data that wants to be sent.


			//	Send_Loop_END) Restart send timer.
			timer_restart(&data_send_timer);

		}//else{} // Send timer has NOT expired.
	}
	else{// There is NOT a connection at picoIPv6 level.
		picoIPv6Connection.connect(); // We initialize a picoIPv6 connection.
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
