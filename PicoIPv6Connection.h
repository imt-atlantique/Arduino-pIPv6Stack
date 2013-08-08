#ifndef PICOIPV6CONNECTION_H_
#define PICOIPV6CONNECTION_H_

#include "arduino_debug.h"
#include "MACLayer.h"
#include "PicoIPv6State.h"
#include "PicoIPv6StateSearchingDIOState.h"
#include "PicoIPv6StateSendingDAOState.h"
#include "PicoIPv6StateConnectedState.h"

#define TIMEOUT_RECEIVING_DIO 8000

#define TIMEOUT_RECEIVING_DAO_ACK 8000

/**
 * \brief SET LINK LAYER ADDRESS TO 0 (MEANS BROADCAST 0xFFFF FOR THIS PURPOSES)
 */
#define uip_set_lladdr_broadcast(a)  do {                    \
    (a)->addr[0] = 0;                                       \
    (a)->addr[1] = 0;                                       \
    (a)->addr[2] = 0;                                       \
    (a)->addr[3] = 0;                                       \
    (a)->addr[4] = 0;                                       \
    (a)->addr[5] = 0;                                       \
    (a)->addr[6] = 0;                                       \
    (a)->addr[7] = 0;                                       \
 } while(0)
 
/** \brief set IP address a to the link local all-nodes multicast address */
#define uip_create_linklocal_allnodes_mcast(a) uip_ip6addr(a, 0xff02, 0, 0, 0, 0, 0, 0, 0x0001)

/* This function needs to be defined in an external file */
/* This function will be automatically invoked when we call PicoIPv6Connection->receive() method
 * and there is new UDP Data.
 * 		* udp_data:	contains new UDP datagram payload.
 * 		* length: 	contains new UDP datagram payload lenght.
 *
 * 	[	At lower level it is actually called from PicoIPv6State.udp_input() ]
 * 	[	only when the PicoIPv6State subclass is ConnectedState				]
 *
 * */
extern void udp_input_callback(uint8_t* udp_data, uint16_t length);

class PicoIPv6Connection{
  private:
    PicoIPv6State* state;
    u8_t* buffer;
    MACLayer* mac;

    uip_lladdr_t* own_ll_address_ptr;
    uip_lladdr_t own_ll_address;
    
    //Gateway
    uip_ipaddr_t gateway_ip_addr;
    uip_lladdr_t gateway_ll_addr;
    
    uip_ipaddr_t link_local_addr;
    
	void initClassPrivate(MACLayer* mac, u8_t* buffer){
		this->mac = mac;

		own_ll_address_ptr = &own_ll_address; // Bugfix for having reserved Memory on own_ll_address_ptr
		memcpy(this->own_ll_address_ptr, mac->getMacAddress(), UIP_LLADDR_LEN); // Solved BUG: memcpy over own_ll_address with no reserved memory, solution:  own_ll_address_ptr = &own_ll_address;

		this->buffer = buffer;

		uip_create_linklocal_allnodes_mcast(&link_local_addr);
		uip_create_linklocal_rplnodes_mcast(&gateway_ip_addr);
		uip_set_lladdr_broadcast(&gateway_ll_addr);

		this->state = new PicoIPv6StateSearchingDIOState(mac, buffer, own_ll_address_ptr, TIMEOUT_RECEIVING_DIO, &gateway_ll_addr, &gateway_ip_addr);
		this->state->setBuffer(buffer);
	}

  public:
	PicoIPv6Connection(){};
    PicoIPv6Connection(MACLayer* mac, u8_t* buffer){
    	initClassPrivate(mac, buffer);
    }
    
    void init( MACLayer* mac , u8_t* buffer ){
    	initClassPrivate(mac, buffer);
    }

    ConnectionState connect(){
      uint8_t instance_id;
      switch (this->state->connect()){
        case WAITING_DIO:
          //if we are waiting for a DIO, we should send again a DIS and continue waiting (TODO: INCREMENT THE TIMEOUT FOR RECEIVING DIO?)
          if (!this->state->has_timer_expired()){
             //if timer did not expire, we need to continue waiting
             PRINTF("WAIT DIO. TIMER NOT EXPIRED");
             break;
          }
        case DISCONNECTED:
          PRINTF("LIKE DISCONNECTED");
           //if we are disconnected, start connection by searching for DIO (send DIS and wait)
          delete(this->state);
          this->state = new PicoIPv6StateSearchingDIOState(mac, buffer, own_ll_address_ptr, TIMEOUT_RECEIVING_DIO, &gateway_ll_addr, &gateway_ip_addr);
          PRINTF("NEW SEARCH DIO STATE.. RECONNECTING");
          this->connect();
          break;
        
        case WAITING_DAO_ACK:
          //if we are waiting for a DAO ACK, we should send again a DAO and continue waiting (TODO: INCREMENT THE TIMEOUT FOR RECEIVING DAO ACK? ALSO, IS IT NECESSARY TO RESTART AFTER WAITING TOO LONG?)
          if (!this->state->has_timer_expired()){
             //if timer did not expire, we need to continue waiting
             break;
          }
        case DIO_RECEIVED:
          //if we received a DIO, it is time to send a DAO, change the state!
          instance_id = this->state->getInstanceId();
          if (instance_id != -1){
            delete(this->state);
            this->state = new PicoIPv6StateSendingDAOState(mac, buffer, own_ll_address_ptr, TIMEOUT_RECEIVING_DAO_ACK, &gateway_ll_addr, &gateway_ip_addr, instance_id);
          }else{
            //We did not actually received a valid DIO cause we do not have the instance ID
            this->state->changeState(DISCONNECTED);
            //retry to connect?
            //this->connect();
          }
          this->connect();
          break;
          
        case DAO_ACK_RECEIVED:
          //In this case, change the state for a CONNECTED STATE
          instance_id = this->state->getInstanceId();
          delete(this->state);
          this->state = new PicoIPv6StateConnectedState(mac, buffer, own_ll_address_ptr, /*0,*/ &gateway_ll_addr, &gateway_ip_addr, &udp_input_callback, instance_id); //by now, we do not use timers when connected
          break;
        
        case CONNECTED:
          //Do nothing (we are already connected!) (TODO: SHOULD WE CHECK IF CONNECTION IS STILL ALIVE?)
          break;  
          
        }
        return this->state->getState();
    }
    
    boolean isConnected(){ return getConnectionState() == CONNECTED; }
    
    void udp_send(uip_ipaddr_t *dest, u16_t src_port, u16_t dest_port, char* payload, u16_t payload_len){
      this->state->udp_send(dest, src_port, dest_port, payload, payload_len);
    }
    
    ConnectionState getConnectionState(){ return this->state->getState(); }
    
    bool receive(){ return this->state->receive(); }
    
    uip_ipaddr_t* getLastSenderIPAddress(){ return this->state->getLastSenderIPAddress(); } // Get last sender IP Address
    uint16_t getLastSenderUDPSourcePort(){ return this->state->getLastSenderUDPSourcePort(); } // Get last UDP sender UDP source Port
	uint16_t getUdpDataLength() { return this->state->getUdpDataLength(); } // Get last UDP packet data/payload size
	uint16_t getUdpData(char * buffer) { return this->state->getUdpData(buffer); } // Get UDP data/payload, copies into udp_msg_buf, return bytes of data copied

	/* Returns the Local Multicast IPv6 Address */
    uip_ipaddr_t* getLinkLocalMcastIPAddress(){ return &this->link_local_addr; }

    ReceivedPacketType getLastReceivedPacketType(){ return this->state->getLastReceivedPacketType();}
};


#endif /* PICOIPV6CONNECTION_H_ */
