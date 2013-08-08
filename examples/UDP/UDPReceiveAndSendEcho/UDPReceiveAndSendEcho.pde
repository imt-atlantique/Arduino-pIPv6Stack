// Do not remove the include below
#include "UDPReceiveAndSendEcho.h"

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
uip_ipaddr_t* ip_destination_address = 0; // IPv6 Destination Address

// Definition of UDP message payload buffer and messages counter.
char udp_msg_buf[UIP_UDP_PAYLOAD_MAX_BUFSIZE];	// UDP payload buffer.
u16_t udp_msg_length;							// UDP payload size
char udp_msg_aux[UIP_UDP_PAYLOAD_MAX_BUFSIZE]; 	// Auxiliar UDP payload buffer

void loop() {

	if (picoIPv6Connection->isConnected()){
		// There is a connection at picoIPv6 level.

		if (picoIPv6Connection->receive()){//We answer the message we have just reverted in the msg_buffer

		  if (picoIPv6Connection->getLastReceivedPacketType() == UDP){

			// 1) Get UDP received packet Payload Length and Payload
			// udp_msg_length = picoIPv6Connection->getUdpDataLength();		// Method also available to get UDP Payload Lenght
			udp_msg_length = picoIPv6Connection->getUdpData(udp_msg_aux);	// Copies UDP Payload into udp_msg_aux and returns payload lenght.
			udp_msg_aux[udp_msg_length] = '\0'; // We NULL Terminate the string to make possible to sprintf(,"%s",) correctly detect the end of the string.

			// 2) Create new UDP Payload
			// We generate udp_msg_buf indicating received payload lenght and sending/echoing the payload again.
			sprintf(udp_msg_buf, "ACK.PayLen.%hu:%s\n", udp_msg_length, udp_msg_aux);	// Generate the payload
			udp_msg_length = strlen(udp_msg_buf); 										// Get new payload lenght

			// 3) Send UDP message to Last Sender (IP address and UDP Port)
			ip_destination_address 	= picoIPv6Connection->getLastSenderIPAddress();		// Get Last Sender's IP Address.
			udp_destination_port 	= picoIPv6Connection->getLastSenderUDPSourcePort();	// Get Last Sender's UDP Source Port.
			picoIPv6Connection->udp_send(ip_destination_address , udp_source_port, udp_destination_port, udp_msg_buf, udp_msg_length ); // Send UDP message.

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
void udp_input_callback(uint8_t* udp_data, uint16_t length){}

