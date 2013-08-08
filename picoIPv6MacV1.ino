#include "XBeeMACLayer.h"
#include "PicoIPv6Connection.h"
//#include "CoAPEngine.h"
#define DEBUG 0

int mem(){
  uint8_t * heapptr, * stackptr;
  stackptr = (uint8_t *)malloc(4);
  heapptr = stackptr;
  free(stackptr);               
  stackptr = (uint8_t *)(SP);
  return stackptr - heapptr;
}

u8_t buf[UIP_BUFSIZE];

/*-------------------------------------------------------------------------------------*/


#define UDP_CLIENT_PORT 8765

XBeeMACLayer mac;

PicoIPv6Connection* connection;
//CoAPEngine coap;						// CoAPEngine-alpha. Sends CoAP PUT Non-Confirmable messages with URI, Query, optional second Query, and payload.


//struct timer udp_msg_timer;

void setup() {    
  Serial.begin(9600);
  delay(1000);
  
  if (!mac.init()){
    PRINTF("CANNOT INITIALIZE XBEE MODULE.. CANNOT CONTINUE");
    while (1){};
  }
  
  Serial.println("MAC ADDRESS: 0x");
  for (int i=0; i<8; ++i){
	  Serial.println( mac.getMacAddress()->addr[i], HEX);
  }
 
  Serial.println("MACOK");

  connection = new PicoIPv6Connection(&mac, buf);
  
  Serial.println("CONOK");

  //Send one message to broadcast
  char aux[] = "HELLO";
  connection->udp_send(connection->getLinkLocalMcastIPAddress(), UDP_CLIENT_PORT, UDP_CLIENT_PORT, aux , 5);
  
  Serial.println("MCASTOK");

  //Serial.println();
  //Serial.println();
  PRINTF("MEMORY RAM LEFT: ");
  Serial.println(mem(), DEC);
  //Serial.println();
  delay(100);
  //timer_set(&udp_msg_timer, UDP_SEND_TIME);
}

char msg_buf[160];

uint16_t msg_length;

void udp_input_callback(uint8_t* udp_data, uint16_t length){  
  uint8_t i;
  
  PRINTF();
  PRINTF();
  PRINTF("DATA rcv: ");
  for (i=0; i < length; ++i){
    PRINTHEX((char)udp_data[i]);
  }
  PRINTF();
  PRINTF();
  
  for(i=0; i<length; ++i){
    msg_buf[length-1-i] = (char) udp_data[i]; 
  }
  
  msg_length = length;
  
//  Serial.println();
  PRINTF("MEM LEFT: ");
//  Serial.println(mem());
  
}


void loop() {
  if (connection->isConnected()){
    PRINTF("CONNECTED!");
    Serial.println("CONN!");
    delay(100);
    /*
    if (timer_expired(&udp_msg_timer)){
      PRINTF("UDP TIMER EXPIRED! SEND..");
      ++msg_counter;
      sprintf(msg_buf, msg, msg_counter);
      connection->udp_send(&remote_client_ipaddr, UDP_CLIENT_PORT, UDP_CLIENT_PORT, msg_buf, strlen(msg_buf));
      timer_restart(&udp_msg_timer);
    }else{
      PRINTF("UDP TIMER NOT EXPIRED.");
    }
    */
    if (connection->receive()){//We answer the message we have just reverted in the msg_buffer
      PRINTF("RECEIVED!");
      if (connection->getLastReceivedPacketType() == UDP){
        PRINTF();
        PRINTF();
        PRINTF("IT IS UDP MESSAGE!!");
        connection->udp_send(connection->getLastSenderIPAddress(), UDP_CLIENT_PORT, UDP_CLIENT_PORT, msg_buf, msg_length);
      //  Serial.println("");
      //  Serial.println(mem(), DEC);

        delay(100);
        msg_buf[msg_length] = 0; //in order to print it for debugging, we finish the string with a 0
        PRINTF();
        PRINTF("MSG received: ");
        PRINTF(msg_buf);
        PRINTF();
        PRINTF();
      }
    }
    
      //delay(UDP_SEND_TIME);
  }else{

	Serial.println("NOC");
	Serial.println(mem(), DEC);
    connection->connect();
    delay(200);
    if (connection->isConnected()){//We have just connected for the first time or from a disconnection
      //timer_restart(&udp_msg_timer);
      //PRINTF("NEW UDP SENDING TIMER SET");      
      PRINTF("CONNECTED!");
    }
  }
  delay(200);
}

    
