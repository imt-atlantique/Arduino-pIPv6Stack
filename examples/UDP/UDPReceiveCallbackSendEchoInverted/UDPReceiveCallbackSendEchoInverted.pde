// Do not remove the include below
#include "UDPReceiveCallbackSendEchoInverted.h"

#include "XBeeMACLayer.h"
#include "PicoIPv6Connection.h"


XBeeMACLayer macLayer; 					// XBee MAC Layer
PicoIPv6Connection* picoIPv6Connection;	// PicoIPv6 Connection
u8_t picoip_buffer[UIP_BUFSIZE]; 		// Defines the picoIPv6 packet buffer.

void setup() {

	Serial.begin(9600);

	// Start the MACLayer and PicoIPv6 Connection:
	// 	1) Initialization of the MAC Layer.
	if ( !macLayer.init() ) {
		 while (1){}; // Cannot initialize MAC Layer and will not continue.
	}

	// 2) Initialization of the picoIPv6 Connection.
	picoIPv6Connection = new PicoIPv6Connection(&macLayer, picoip_buffer); // Definition of an object of the class PicoIPv6Connection

}


// Definitions for the UDP Message to be sent
const u16_t udp_source_port = 8765; // UDP Source Port Number.
u16_t udp_destination_port	= 8765; // UDP Destination Port Number.

// Definition of UDP message payload buffer and messages counter.
char udp_msg_buf[UIP_UDP_PAYLOAD_MAX_BUFSIZE];	// UDP payload buffer.
u16_t udp_msg_length;

void loop() {

	if (picoIPv6Connection->isConnected()){
		// There is a connection at picoIPv6 level.

		if (picoIPv6Connection->receive()){//We answer the message we have just reverted in the msg_buffer

		  if (picoIPv6Connection->getLastReceivedPacketType() == UDP){

			/* 0) The function udp_input_callback has already been invoked by picoIPv6Connection->receive() because this is an UDP packet
			 * 		On udp_input_callback we can make all preprocessing needed to the UDP payload we just received.
			 *
			 * 		On the current example we inverted the payload content and copied it to udp_msg_buf,
			 * 		also setting the proper payload lenght to udp_msg_length
			 */


			// 1) Send UDP message to Last Sender (IP address and UDP Port)
			udp_destination_port = picoIPv6Connection->getLastSenderUDPSourcePort(); // Get Last Sender's UDP Source Port.
			picoIPv6Connection->udp_send(picoIPv6Connection->getLastSenderIPAddress(), udp_source_port, udp_destination_port, udp_msg_buf, udp_msg_length); // Send UDP message to Last Sender's IP Address.

			delay(100);
		  }
		}

	}else{
		// There is NOT a connection at picoIPv6 level.
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
void udp_input_callback(uint8_t* udp_data, uint16_t length){

	// We will define a function that inverts udp_data string and put it into udp_msg_buf
	// Also we will set the new udp_msg_length = length
	uint8_t i;

	for(i=0; i<length; ++i){
		udp_msg_buf[length-1-i] = (char) udp_data[i];
	}

	udp_msg_length = length;

}

