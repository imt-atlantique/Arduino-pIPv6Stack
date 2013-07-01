
#ifndef __XBEEMACLAYER__
#define __XBEEMACLAYER__

#include "XBee.h"
#include "MACLayer.h"
#include "arduino_debug.h"

class XBeeMACLayer: public MACLayer{
    private:
      XBee xbee;
      uip_lladdr_t my_mac;
      
      bool sendAtCommand();
      bool getResponseMAC();
      bool getResponseCCAFailure();
      int getNumberOfTransmissions();
      
    public:      
      XBeeMACLayer();
      bool init();
      MACTransmissionStatus send(const uip_lladdr_t* lladdr_dest, uint8_t* data, uint16_t length, int *number_transmissions);
      virtual bool receive(uip_lladdr_t* lladdr_src, uip_lladdr_t* lladdr_dest, uint8_t* data, uint16_t* length);
      const uip_lladdr_t* getMacAddress();
};

#endif
