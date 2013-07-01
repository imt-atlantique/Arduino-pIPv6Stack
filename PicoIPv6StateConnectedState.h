#ifndef __CONNECTEDSTATE_H__
#define __CONNECTEDSTATE_H__

#include "PicoIPv6State.h"

#define DEBUG 0

class PicoIPv6StateConnectedState : public PicoIPv6State{
  private:
  
    virtual void process_input();
    
  public:
  
    PicoIPv6StateConnectedState(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, /*int timeout, */uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address, void (* udp_input_callback)(uint8_t* udp_data, uint16_t length));
    
    virtual ConnectionState connect();
    
    virtual bool has_timer_expired();
  
};

#endif
