#ifndef __SEARCHDIOSTATE_H__
#define __SEARCHDIOSTATE_H__


extern "C"{
  #include "timer.h" 
}

#include "PicoIPv6State.h"

#define DEBUG 0

class PicoIPv6StateSearchingDIOState : public PicoIPv6State{
  private:
    void dis_output(uip_ipaddr_t *addr);
    
    bool receive_DIO();
    
    void dio_input();
    
    int timeout;
    
    virtual void process_input();
    
    struct timer wait_dio_timer;

  public:
    
      PicoIPv6StateSearchingDIOState(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, int timeout, uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address);
    
      virtual ConnectionState connect();
      
      virtual bool has_timer_expired();
};

#endif
