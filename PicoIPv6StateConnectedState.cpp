#include "PicoIPv6StateConnectedState.h"

PicoIPv6StateConnectedState::PicoIPv6StateConnectedState(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, /*int timeout,*/ uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address, void (* udp_input_callback)(uint8_t* udp_data, uint16_t length)):PicoIPv6State(mac, buffer, own_ll_address, gateway_ll_address, gateway_ip_address){
        this->currentState = CONNECTED;
        this->udp_input_callback = udp_input_callback;
}

void PicoIPv6StateConnectedState::process_input(){
  // DO NOTHING BY NOW 
}

ConnectionState PicoIPv6StateConnectedState::connect(){
  //We are already connected! (TODO: discuss if we have to validate connection every X time. Could use the timer for that)
  return currentState;
}

bool PicoIPv6StateConnectedState::has_timer_expired(){
  //By now we say that it has expired because there is nothing to check
  return true;
}
