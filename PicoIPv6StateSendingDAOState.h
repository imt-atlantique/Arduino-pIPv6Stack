#ifndef __SENDINGDAOSTATE_H__
#define __SENDINGDAOSTATE_H__

extern "C"{
  #include "timer.h" 
}

#include "PicoIPv6State.h"

#define DEBUG 0

class PicoIPv6StateSendingDAOState : public PicoIPv6State{
  private:
    void dao_output(uip_ipaddr_t *addr, rpl_lifetime_t lifetime, uint8_t instance_id);
    
    bool receive_DAO_ACK();
    
    int timeout;
    
    static uint8_t dao_sequence;
    
    struct timer wait_dao_ack_timer;
    
  protected:
  
    virtual void process_input();

  public:
    
      PicoIPv6StateSendingDAOState(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, int timeout, uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address, uint8_t instance_id);
    
      virtual ConnectionState connect();
      
      virtual bool has_timer_expired();
      
};

#endif
