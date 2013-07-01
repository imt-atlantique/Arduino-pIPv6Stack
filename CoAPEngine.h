/*
 * CoAPEngine.h
 *
 *  Created on: Apr 12, 2013
 *      Author: rnavas
 */

#ifndef COAPENGINE_H_
#define COAPENGINE_H_

#include "Arduino.h"
#include "PicoIPv6Connection.h"



class CoAPEngine {
private:
	PicoIPv6Connection picoIPv6Connection;

	static const u16_t coap_port 	= 5683; 	// CoAP packets are sent 5683 by default.
	static const u16_t source_port 	= 61616;	// UDP Source Port Number. Chosed 61616 (0xF0B0) for 6lowpan compression from 16 to 4 bits http://tools.ietf.org/html/rfc6282#section-4.3.1
	static const int COAP_PACKET_SIZE = UIP_UDP_PAYLOAD_MAX_BUFSIZE;	// CoAP packet should be less than UIP_UDP_PAYLOAD_MAX_BUFSIZE 112 bytes in this example
	char packetBuffer[ COAP_PACKET_SIZE ];		// Buffer to hold outgoing packets
	word messageID;

public:
	CoAPEngine();
	void init(const PicoIPv6Connection& picoIPv6Connection);

	const PicoIPv6Connection& getPicoIPv6Connection() const { return picoIPv6Connection; }
	void setPicoIPv6Connection(const PicoIPv6Connection& picoIPv6Connection) { this->picoIPv6Connection = picoIPv6Connection; }

	void sendCoAPpacket(uip_ipaddr_t& address, const String& uri, const String& query, const String& secondQuery, const String& payload);


	};

#endif /* COAPENGINE_H_ */
