
#include "XBeeMACLayer.h"

#define XBEE_INIT_WAITING_TIME  3000 // XBee initialization waiting time

XBeeMACLayer::XBeeMACLayer(){
}

// serial high
uint8_t shCmd[] = {'S','H'};
// serial low
uint8_t slCmd[] = {'S','L'};
// cca failures
uint8_t ecCmd[] = {'E','C'};

uint8_t mac_position;

AtCommandRequest atRequest = AtCommandRequest(shCmd);
AtCommandResponse atResponse = AtCommandResponse();

#define AT_RESPONSE_MAX_ATTEMPTS 5
#define AT_REQUEST_MAX_ATTEMPTS 20

bool XBeeMACLayer::getResponseMAC(){
          if (atResponse.getValueLength() == 4) {            
            for (int i = 0; i < atResponse.getValueLength(); i++) {
              my_mac.addr[mac_position++] = atResponse.getValue()[i];
            }
            return true;
          }
          return false;
}

uint16_t cca_retries = 0;

bool XBeeMACLayer::getResponseCCAFailure(){
          if (atResponse.getValueLength() == 2) {            
            for (int i = 0; i < atResponse.getValueLength(); i++) {
              ((char*)(&cca_retries))[1] = atResponse.getValue()[0];
              ((char*)(&cca_retries))[0] = atResponse.getValue()[1];
            }
            return true;
          }
          return false;
}

bool XBeeMACLayer::sendAtCommand() {
  for(int i=0; i<AT_REQUEST_MAX_ATTEMPTS; ++i){
    // send the command
    xbee.send(atRequest);
  
    //TEMPORARILY, we send the request and wait for the attempt several times until we got it. This is because we can receive other packets that will be placed in the same buffer and they must be ignored until getting our AT response
    for(int i=0; i<AT_RESPONSE_MAX_ATTEMPTS; ++i){
      
      // wait up to 3 seconds for the status response
      if (xbee.readPacket(1000)) {
        // got a response!
    
        // should be an AT command response
        if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
          xbee.getResponse().getAtCommandResponse(atResponse);
    
          if (atResponse.isOk()) {
            if (atRequest.getCommand() == shCmd || atRequest.getCommand() == slCmd){
              return getResponseMAC();
            }else{
              if (atRequest.getCommand() == ecCmd){
                 return getResponseCCAFailure();
              }
            }
          } 
        } 
      } 
    }
  }
  return false;
}

bool XBeeMACLayer::init(){    

	xbee.begin(9600);
	delay(XBEE_INIT_WAITING_TIME); //wait for the XBEE to initialize

	mac_position = 0;
	if (!sendAtCommand()) return false;	// get SH (Serial High)
	atRequest.setCommand(slCmd);		// Set command to SL
	if (!sendAtCommand()) return false;	// get SL (Serial Low)

	return true;
}


XBeeAddress64 destAddr64(0, 0);

uint32_t dest = 0;
uint16_t dest16 = 0;

#define XBEE_SEND_OK 0x0
#define XBEE_SEND_NO_ACK 0x1
#define XBEE_SEND_COLLISION 0x2
#define XBEE_SEND_PURGED 0x3 //This happens when Coordinator times out of an indirect transmission (should not use coordinators in our case)

TxStatusResponse txStatus;

uint16_t ecCommandValue = 0; //reset counter to 0

#define CCA_RETRIES_FAIL 10;

int XBeeMACLayer::getNumberOfTransmissions(){
      atRequest.setCommand(ecCmd); 
      atRequest.setCommandValue((uint8_t*) &ecCommandValue);
      if (!sendAtCommand())
        return CCA_RETRIES_FAIL; 
      return cca_retries;
}


XBeeRequest* xbeeRequest;
MACTransmissionStatus XBeeMACLayer::send(const uip_lladdr_t* lladdr_dest, uint8_t* data, uint16_t length, int *number_transmissions){
  unsigned long send_start_time;
  unsigned int sending_time;
  uint8_t status;

  // Broadcast is expressed as an all-zeroes address (rimeaddr_null), we check to see if lladdr_dest is a broadcast address comparing it to rimeaddr_null.
  bool isNotBroadcast = !rimeaddr_cmp((rimeaddr_t*)lladdr_dest, &rimeaddr_null, UIP_LLADDR_LEN); // lladdr_dest != NULL

    if (isNotBroadcast){
      // lladdr_dest is NOT a broadcast address
      if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN){
          ((char*)(&dest))[3] = lladdr_dest->addr[0];
          ((char*)(&dest))[2] = lladdr_dest->addr[1];
          ((char*)(&dest))[1] = lladdr_dest->addr[2];
          ((char*)(&dest))[0] = lladdr_dest->addr[3];
          destAddr64.setMsb(dest);
          ((char*)(&dest))[3] = lladdr_dest->addr[4];
          ((char*)(&dest))[2] = lladdr_dest->addr[5];
          ((char*)(&dest))[1] = lladdr_dest->addr[6];
          ((char*)(&dest))[0] = lladdr_dest->addr[7];
          destAddr64.setLsb(dest);
        Tx64Request longReq(destAddr64, data, length);
        xbeeRequest = &longReq;
      }else{
        ((char*)(&dest))[1] = lladdr_dest->addr[0];
        ((char*)(&dest))[0] = lladdr_dest->addr[1];
        Tx16Request shortReq(dest16, data, length);
        xbeeRequest = &shortReq;
      }
    }else{// lladdr_dest is a broadcast address, we send to broadcast
      Tx16Request shortReq(0xFFFF, data, length);
      xbeeRequest = &shortReq;
    }
    send_start_time = millis();
    xbee.send(*xbeeRequest);


    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    for(int i=0; i<AT_RESPONSE_MAX_ATTEMPTS; ++i){
      if (xbee.readPacket(5000)) {
          // got a response!
          // should be a znet tx status            	
      	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      	   xbee.getResponse().getTxStatusResponse(txStatus);
      	   // get the delivery status, the fifth byte
             switch (txStatus.getStatus()) {
               case XBEE_SEND_OK:
                 *number_transmissions = getNumberOfTransmissions();
                 ++number_transmissions;//we have always one (the first one), plus the number of times that we had cca failure (collision)

                 return MAC_TX_STATUS_OK;
               case XBEE_SEND_NO_ACK:
                 return MAC_TX_STATUS_NO_ACK;
               case XBEE_SEND_COLLISION:
                 *number_transmissions = getNumberOfTransmissions();
                 ++number_transmissions;//we have always one (the first one), plus the number of times that we had cca failure (collision)

                 return MAC_TX_STATUS_COLLISION;
               case XBEE_SEND_PURGED:
               default:
                 return MAC_TX_STATUS_ERR;
             }     
          }   
      }
    }
    return MAC_TX_STATUS_ERR;

}

ZBRxResponse rxResp = ZBRxResponse();

bool XBeeMACLayer::receive(uip_lladdr_t* lladdr_src, uip_lladdr_t* lladdr_dest, uint8_t* data, uint16_t* length){
    xbee.readPacket();    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
          if (UIP_LLADDR_LEN == UIP_802154_SHORTADDR_LEN){
                Rx16Response rx16 = Rx16Response();                
                xbee.getResponse().getRx16Response(rx16);
                memcpy(data, rx16.getData(), rx16.getDataLength());
                *length = rx16.getDataLength();
                
                //get the src and destination addresses
                if(rx16.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF, we use an all-zeroes address to represent broadcast
                  lladdr_dest->addr[0] = 0; // Copy all-zeroes address to lladdr_dest
                  lladdr_dest->addr[1] = 0;
                  //lladdr_dest = NULL;

                }else{
                  //get dest address (it is my own)
                  //lladdr_dest = &my_mac;
                  lladdr_dest->addr[0] = my_mac.addr[0]; // Copy my_mac address to lladdr_dest
                  lladdr_dest->addr[1] = my_mac.addr[1];
                }
                
                lladdr_src->addr[0] = rxResp.getRemoteAddress16() >> 8;
                lladdr_src->addr[1] = rxResp.getRemoteAddress16() & 0x00FF;
          }else
            return false;
        } else {           
            if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN){
                Rx64Response rx64 = Rx64Response();
                xbee.getResponse().getRx64Response(rx64);
                memcpy(data, rx64.getData(), rx64.getDataLength());
                *length = rx64.getDataLength();
                                
                //get destination addresses
                if(rx64.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF, we use an all-zeroes address to represent broadcast
					//lladdr_dest = NULL;
                	rimeaddr_copy((rimeaddr_t*)lladdr_dest , &rimeaddr_null, UIP_LLADDR_LEN); // Copy an all-zeroes address to lladdr_dest
                }else{
					//get dest address (it is my own)
					//lladdr_dest = &my_mac;
					rimeaddr_copy((rimeaddr_t*)lladdr_dest , (rimeaddr_t*)(&my_mac), UIP_LLADDR_LEN); // Copy my_mac address to lladdr_dest
                }
                
                //get sorce address
                for(int i=0; i<UIP_LLADDR_LEN; ++i){
                  lladdr_src->addr[i] = rx64.getFrameData()[i];
                }
                
             }else
               return false;
        }
                
        return true;
        
      }
    } else if (xbee.getResponse().isError()) {
      uint8_t code = xbee.getResponse().getErrorCode();
      if (code == CHECKSUM_FAILURE){
      }else if (code == PACKET_EXCEEDS_BYTE_ARRAY_LENGTH){
      }else if(code == UNEXPECTED_START_BYTE ){
      }
    } 
    
    return false;
}

const uip_lladdr_t* XBeeMACLayer::getMacAddress(){
  return &my_mac;
}
