/*
 * CoAPEngine.cpp
 *
 *  Created on: Apr 12, 2013
 *      Authors: Baptiste Gaultier, Renzo Navas
 */

#include "CoAPEngine.h"

CoAPEngine::CoAPEngine() {}

// Input: an initialized picoIPv6 Connection
void CoAPEngine::init(const PicoIPv6Connection& picoIPv6Connection) {
		setPicoIPv6Connection(picoIPv6Connection);
}


/* sendCoAPpacket function arguments :
  * IPAddress& address : IP adress of the CoAP server
  * String uri         : URI you want to access ('/' are not needed)
  * String query       : URI query optionnal, example : ?query=1
  * String query       : URI query optionnal, example : &query=2
  * String payload     : the actual payload
*/

// emoncms API
/*
  * uri → input name
  * query → apikey (you have to specify your Read & Write API key
  * query → node number (optionnal)
  * payload → sensor reading you want to send
*/

// send an CoAP request to the time server at the given address
void CoAPEngine::sendCoAPpacket(uip_ipaddr_t& address, const String& uri, const String& query, const String& secondQuery, const String& payload) {
  // Based on draft-ietf-core-coap-13 :  http://tools.ietf.org/html/draft-ietf-core-coap-13
  //memset(packetBuffer, 0, COAP_PACKET_SIZE * sizeof(byte));

  //packetBuffer = udp_msg_buf; // hack for not changin nae


  // Where we are in the packet
  byte index = 0;

  // Message Format → http://tools.ietf.org/html/draft-ietf-core-coap-13#section-3

  // Initialize values needed to form CoAP header
  packetBuffer[index++] = 0b01010000;  // Version : 1 (01), Type  : Non-Confirmable (01), Option Count : 0 (0000)
  packetBuffer[index++] = 0b00000011;  // Code : PUT (00000011)


  // Increment messageID
  this->messageID++;

  packetBuffer[index++] = highByte(this->messageID);  // Message ID high byte
  packetBuffer[index++] = lowByte(this->messageID);   // Message ID low byte

  // options
  packetBuffer[index++] = 0xB0 | uri.length(); // Uri-Path : 11� http://tools.ietf.org/html/draft-ietf-core-coap-13#section-5.10

  byte i;
  for (i = 0; i < uri.length(); i++) {
    packetBuffer[i + index] = uri.charAt(i);
  }
  index += i;

  if(query.length() <= 12)
    packetBuffer[index++] = 0x40 | query.length();  // Uri-Query : 15  ( (DELTA=11) + 4)
  else {
    // lenght is greater than 12
    packetBuffer[index++] = 0x4D;                   // Uri-Query : 15, Lenght : 13( An 8-bit unsigned integer precedes
    packetBuffer[index++] = query.length() - 13;    // the Option Value and indicates the Option Length minus 13.
  }

  // uri-query
  for (i = 0; i < query.length(); i++) {
    packetBuffer[i + index] = query.charAt(i);
  }
  index += i;

  if (secondQuery.length() != 0 ) {

	  if(secondQuery.length() <= 12)
		packetBuffer[index++] = 0x00 | secondQuery.length();  // Uri-Query : 15
	  else {
		// lenght is greater than 12
		packetBuffer[index++] = 0x0D;                         // Uri-Query : 15, Lenght : 13 A 8-bit unsigned integer precedes
		packetBuffer[index++] = secondQuery.length() - 13;    // the Option Value and indicates the Option Length minus 13.
	  }

	  // uri-query
	  for (i = 0; i < secondQuery.length(); i++) {
		  packetBuffer[i + index] = secondQuery.charAt(i);
	  }

	  index += i;
  }

  //****************************************
  // PAYLOAD

  packetBuffer[index++] = 0xFF;  // Payload Marker (0xFF)

  //payload
  for (i = 0; i < payload.length(); i++) {
    packetBuffer[i + index] = payload.charAt(i);
  }
  index += i;

  // all CoAP fields have been given values, now
  // you can send a packet requesting a PUT request

  picoIPv6Connection.udp_send(&address, source_port, coap_port, packetBuffer, index );


}
