#include "PicoIPv6StateSendingDAOState.h"
#include "arduino_debug.h"

uint8_t PicoIPv6StateSendingDAOState::dao_sequence = 0;

#define PREFIX_LENGTH 8

u8_t prefix[PREFIX_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0};

void PicoIPv6StateSendingDAOState::dao_output(uip_ipaddr_t *addr_dest, rpl_lifetime_t lifetime, uint8_t instance_id)
{
  unsigned char *buffer;
  uint8_t prefixlen;
  uip_ipaddr_t addr;
  int pos;

  buffer = UIP_ICMP_PAYLOAD;

  ++dao_sequence;
  pos = 0;

  buffer[pos++] = instance_id;
#if RPL_CONF_DAO_ACK
  buffer[pos++] = RPL_DAO_K_FLAG; /* DAO ACK request, no DODAGID */
#else
  buffer[pos++] = 0; /* No DAO ACK request, no DODAGID */
#endif
  buffer[pos++] = 0; /* reserved */
  buffer[pos++] = dao_sequence & 0xff;

  /* create target subopt */
  prefixlen = PREFIX_LENGTH * CHAR_BIT;
  buffer[pos++] = RPL_OPTION_TARGET;
  buffer[pos++] = 18;//2 + ((prefixlen + 7) / CHAR_BIT);
  buffer[pos++] = 0; /* reserved */
  buffer[pos++] = 128;
  memcpy(buffer + pos, &own_ip_address, 16);
  pos += 16;

  /* Create a transit information sub-option. */
  buffer[pos++] = RPL_OPTION_TRANSIT;
  buffer[pos++] = 4;
  buffer[pos++] = 0; /* flags - ignored */
  buffer[pos++] = 0; /* path control - ignored */
  buffer[pos++] = 0; /* path seq - ignored */
  buffer[pos++] = lifetime;

  icmp6_send(addr_dest, ICMP6_RPL, RPL_CODE_DAO, pos);
}

PicoIPv6StateSendingDAOState::PicoIPv6StateSendingDAOState(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, int timeout, uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address, uint8_t instance_id):PicoIPv6State(mac, buffer, own_ll_address, gateway_ll_address, gateway_ip_address){
  this->dodag_instance_id = instance_id;
  this->currentState = DIO_RECEIVED;
  this->timeout = timeout;  
}

bool PicoIPv6StateSendingDAOState::receive_DAO_ACK(){

	if (receive_mac() == RECEIVED){ //GOT SOMETHING AT MAC LEVEL
        PRINTF("RECEIVED AT MAC LEVEL");

        if (receive_ipv6() != NOT_EXPECTED_OR_ERROR){ //IF WE RECEIVED SOMETHING THAT WE COULD EXPECT, SEE IF IT IS A DIO
          PRINTF("RECEIVED SOMETHING EXPECTED");

          if (lastReceived == DAO_ACK){
            PRINTF("RECEIVED DAO ACK!");
            return true;
          }else{
				if (this->lastReceived == NS){
					//if we are expecting a DAO ACK but we receive an NS, this means that the GW is performing ND with us so we have just sent a NA as response.
					//This means also that if the GW received the NA successfully, it will be able to respond our DAO with a DAO ACK this time, so send a new DAO!
					dao_output(this->gateway_ip_address, RPL_DEFAULT_LIFETIME, this->dodag_instance_id);
					return false; //Because we did not yet receive the DAO ACK
				}
			}
        }
      }
      return false;
}

ConnectionState PicoIPv6StateSendingDAOState::connect(){
        if (dodag_instance_id != -1){
          if (currentState == DIO_RECEIVED){
            dao_output(gateway_ip_address, RPL_DEFAULT_LIFETIME, dodag_instance_id);
            delay(200);
            currentState = WAITING_DAO_ACK;
            timer_set(&wait_dao_ack_timer, timeout);
          }
          if (currentState == WAITING_DAO_ACK && !timer_expired(&wait_dao_ack_timer)){
            if (receive_DAO_ACK()){
              currentState = DAO_ACK_RECEIVED;
            }
          }
        }else{
          //dodag_instance_id not valid! we should remain disconnected
          PRINTF("NOT VALID INSTANCE ID! Changing state to DISCONNECTED");
          currentState = DISCONNECTED;
        }
        return currentState;
}

bool PicoIPv6StateSendingDAOState::has_timer_expired(){
  if (currentState == WAITING_DAO_ACK){
    if (timer_expired(&wait_dao_ack_timer))
      return true;
  }  
  return false;  
}

void PicoIPv6StateSendingDAOState::process_input(){
  //DO NOTHING BY NOW.. 
}
