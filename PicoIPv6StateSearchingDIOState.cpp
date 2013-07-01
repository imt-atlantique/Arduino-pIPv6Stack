
#include "PicoIPv6StateSearchingDIOState.h"
#include "arduino_debug.h"

    void
    PicoIPv6StateSearchingDIOState::dis_output(uip_ipaddr_t *addr)
    {
      unsigned char *buffer;
    
      /* DAG Information Solicitation  - 2 bytes reserved */
      /*      0                   1                   2        */
      /*      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3  */
      /*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
      /*     |     Flags     |   Reserved    |   Option(s)...  */
      /*     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
    
      buffer = UIP_ICMP_PAYLOAD;
      buffer[0] = buffer[1] = 0;

      icmp6_send(addr, ICMP6_RPL, RPL_CODE_DIS, 2);
    }
    
    bool PicoIPv6StateSearchingDIOState::receive_DIO(){
      if (receive_mac() == RECEIVED){ //GOT SOMETHING AT MAC LEVEL
        PRINTF("RECEIVED AT MAC LEVEL");
        if (receive_ipv6() != NOT_EXPECTED_OR_ERROR){ //IF WE RECEIVED SOMETHING THAT WE COULD EXPECT, SEE IF IT IS A DIO
          PRINTF("RECEIVED SOMETHING EXPECTED");
          if (lastReceived == DIO){
            PRINTF("RECEIVED DIO");
            if (dodag_instance_id != -1){
              //DIO IS VALID AND WE GOT ITS INSTANCE ID
              PRINTF("VALID DIO!");
              return true;
            }            
          }
        }
      }
      return false;
    }
    
      PicoIPv6StateSearchingDIOState::PicoIPv6StateSearchingDIOState(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, int timeout, uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address):PicoIPv6State(mac, buffer, own_ll_address, gateway_ll_address, gateway_ip_address){
        this->currentState = DISCONNECTED;
        this->timeout = timeout;
      }
    
    ConnectionState PicoIPv6StateSearchingDIOState::connect(){
      if (currentState == DISCONNECTED){
        dis_output(gateway_ip_address);
        delay(200);
        currentState = WAITING_DIO;
        timer_set(&wait_dio_timer, timeout);
      }
      if (currentState == WAITING_DIO && !timer_expired(&wait_dio_timer)){
        if (receive_DIO()){
          currentState = DIO_RECEIVED;
          //now that we received a DIO, copy the values of gateways! (at layers 3 and 2)
          uip_ipaddr_copy(gateway_ip_address, &src_ip_address);
          uip_lladdr_copy(gateway_ll_address, &src_resp, UIP_LLADDR_LEN);
        }
      }
        
      return currentState;
    }      
          
    /*--------------------------------*/
    void
    PicoIPv6StateSearchingDIOState::dio_input(void)
    {   

      dodag_instance_id = ((unsigned char *)UIP_ICMP_PAYLOAD)[0];
      
    }
    
bool PicoIPv6StateSearchingDIOState::has_timer_expired(){
  if (currentState == WAITING_DIO){//if we do not have the WAITING DIO state, we have not even initialized the timer!
    if (timer_expired(&wait_dio_timer))
      return true;
  }  
  return false; 
}
    
    void PicoIPv6StateSearchingDIOState::process_input(){
      dio_input();
    }
    
