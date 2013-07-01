
#include "arduino_debug.h"
#include "PicoIPv6State.h"
    u16_t
    PicoIPv6State::uip_htons(u16_t val)
    {
      return UIP_HTONS(val);
    }
    
    u32_t
    PicoIPv6State::uip_htonl(u32_t val)
    {
      return UIP_HTONL(val);
    }
    
    u16_t
    PicoIPv6State::chksum(u16_t sum, const u8_t *data, u16_t len)
    {
      u16_t t;
      const u8_t *dataptr;
      const u8_t *last_byte;
    
      dataptr = data;
      last_byte = data + len - 1;
      
      while(dataptr < last_byte) {   /* At least two more bytes */
        t = (dataptr[0] << 8) + dataptr[1];
        sum += t;
        if(sum < t) {
          sum++;      /* carry */
        }
        dataptr += 2;
      }
      
      if(dataptr == last_byte) {
        t = (dataptr[0] << 8) + 0;
        sum += t;
        if(sum < t) {
          sum++;      /* carry */
        }
      }
    
      /* Return sum in host byte order. */
      return sum;
    }
    
    u16_t
    PicoIPv6State::uip_chksum(u16_t *data, u16_t len)
    {
      return uip_htons(chksum(0, (u8_t *)data, len));
    }
    
    u16_t
    PicoIPv6State::uip_ipchksum(void)
    {
      u16_t sum;
    
      sum = chksum(0, uip_buffer, UIP_IPH_LEN);
      return (sum == 0) ? 0xffff : uip_htons(sum);
    }
    
    
    u16_t
    PicoIPv6State::upper_layer_chksum(u8_t proto)
    {
      volatile u16_t upper_layer_len;
      u16_t sum;
      
      upper_layer_len = (((u16_t)(UIP_IP_BUF->len[0]) << 8) + UIP_IP_BUF->len[1]) ;
      
      /* First sum pseudoheader. */
      /* IP protocol and length fields. This addition cannot carry. */
      sum = upper_layer_len + proto;
      /* Sum IP source and destination addresses. */
      sum = chksum(sum, (u8_t *)&UIP_IP_BUF->srcipaddr, 2 * sizeof(uip_ipaddr_t));
    
      /* Sum TCP header and data. */
      sum = chksum(sum, &uip_buffer[UIP_IPH_LEN],
                   upper_layer_len);
        
      return (sum == 0) ? 0xffff : uip_htons(sum);
    }
    
    
    u16_t
    PicoIPv6State::uip_udpchksum(void)
    {
      return upper_layer_chksum(UIP_PROTO_UDP);
    }
    
    u16_t
    PicoIPv6State::uip_icmp6chksum(void)
    {
      return upper_layer_chksum(UIP_PROTO_ICMP6);
      
    }    
    
    /*---------------------------------------------------------------------------*/
    void
    uip_ds6_set_addr_iid(uip_ipaddr_t *ipaddr, uip_lladdr_t * lladdr)
    {
      /* We consider only links with IEEE EUI-64 identifier MAC addresses */
    #if (UIP_LLADDR_LEN == 8)
      memcpy(ipaddr->u8 + 8, lladdr, UIP_LLADDR_LEN);
      ipaddr->u8[8] ^= 0x02;
    #else
    #error uip-ds6.c cannot build interface address when UIP_LLADDR_LEN is not 6 or 8
    #endif
    }
    
    /*---------------------------------------------------------------------------*/
        
    void
    PicoIPv6State::icmp6_send(uip_ipaddr_t *dest, int type, int code, int payload_len)
    {
    
      UIP_IP_BUF->vtc = 0x60;
      UIP_IP_BUF->tcflow = 0;
      UIP_IP_BUF->flow = 0;
      UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
      UIP_IP_BUF->ttl = HOP_LIMIT; //uip_ds6_if.cur_hop_limit;
      UIP_IP_BUF->len[0] = (UIP_ICMPH_LEN + payload_len) >> 8;
      UIP_IP_BUF->len[1] = (UIP_ICMPH_LEN + payload_len) & 0xff;
    
      //copy dest
      memcpy(&UIP_IP_BUF->destipaddr, dest, sizeof(*dest));
      
      //copy src (Link Local for ICMP)
      //memcpy(&UIP_IP_BUF->srcipaddr, &own_ll_ip_address, sizeof(own_ll_ip_address));
      memcpy(&UIP_IP_BUF->srcipaddr, &own_ip_address, sizeof(own_ip_address));
      //We need to put a link local FE80::
      uip_create_linklocal_prefix(&UIP_IP_BUF->srcipaddr);
    
      UIP_ICMP_BUF->type = type;
      UIP_ICMP_BUF->icode = code;
    
      UIP_ICMP_BUF->icmpchksum = 0;
      UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();
    
      uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + payload_len;

      ip_send(dest);
      
    }
    
    void
    PicoIPv6State::udp_send(uip_ipaddr_t *dest, u16_t src_port, u16_t dest_port, char* payload, u16_t payload_len)
    {
    //PRINTF("UDP SEND..");
      UIP_UDPIP_BUF->vtc = 0x60;
      UIP_UDPIP_BUF->tcf = 0;
      UIP_UDPIP_BUF->flow = 0;
      UIP_UDPIP_BUF->proto = UIP_PROTO_UDP;
      UIP_UDPIP_BUF->ttl = HOP_LIMIT; //uip_ds6_if.cur_hop_limit;
      UIP_UDPIP_BUF->len[0] = (UIP_UDPH_LEN + payload_len) >> 8;
      UIP_UDPIP_BUF->len[1] = (UIP_UDPH_LEN + payload_len) & 0xff;
    
    //PRINTF("UIP HEADER READY");
      //copy dest
      memcpy(&UIP_UDPIP_BUF->destipaddr, dest, sizeof(*dest));
      
     //PRINTF("DEST ADDR READY");
      //copy src
       memcpy(&UIP_UDPIP_BUF->srcipaddr, &own_ip_address, sizeof(own_ip_address));
    
   //PRINTF("SRC ADDR READY");
    
      UIP_UDPIP_BUF->srcport = UIP_HTONS(src_port);
      UIP_UDPIP_BUF->destport = UIP_HTONS(dest_port);
            
      UIP_UDPIP_BUF->udplen = UIP_HTONS(UIP_UDPH_LEN + payload_len); //NEED TO PUT THE SAME LENGTH AS THE IP PACKET!! (UDP header + UDP payload length)      
      
      //PRINTF("UDP HEADER READY");
      
      //copy UDP payload
       memcpy(&uip_buffer[UIP_IPUDPH_LEN], payload, payload_len);
       
       //PRINTF("PAYLOAD READY");
    
      UIP_UDPIP_BUF->udpchksum = 0;
      UIP_UDPIP_BUF->udpchksum = ~uip_udpchksum();
      
      //PRINTF("CHECKSUM READY");
      
      uip_len = UIP_IPUDPH_LEN + payload_len;
      
      ip_send(dest);
                  
    }
    
    void PicoIPv6State::ip_send(uip_ipaddr_t *dest){
      if(uip_len == 0) {
        PRINTF("Nothing to send");
        return;
      }
      
      if(uip_len > UIP_LINK_MTU) {
        PRINTF("Packet to big");
        uip_len = 0;
        return;
      }
      if(uip_is_addr_unspecified(dest)){
        PRINTF("Destination address unspecified");
        uip_len = 0;
        return;
      }
      if(!uip_is_addr_mcast(dest)) {
        if (!rimeaddr_cmp((rimeaddr_t*)gateway_ll_address, &rimeaddr_null, UIP_LLADDR_LEN)){
          compress(gateway_ll_address);
        }else{

          PRINTF("Destination address not mcast and no gway");

        }
      }else{
    	//Serial.println("[ip_send] ip_send IP Multicast. SEND LL Broadcast"); // DEBUGGING
    	compress( (uip_lladdr_t*) (&rimeaddr_null) ); // We send a MAC Broadcast. // old code: compress(NULL); --> FAILS because convention for send_at_mac broadcast convention now is all-zeroes address and not a NULL pointer.

      }
      uip_len = 0;
    }
    
    void PicoIPv6State::send_at_mac(uip_lladdr_t* ll_dest_addr){
	  /*
	  // DEBUGGING
	  Serial.println("[SEND] send_at_mac destination MAC ADDRESS (Other's or Broadcast): 0x");
	  for (int i=0; i<8; ++i){ Serial.println( ll_dest_addr->addr[i], HEX); }
	  */
	  int num_trans = 0;
      mac->send(ll_dest_addr, uip_6lp_buf.u8, uip_6lp_len, &num_trans);
    }
    
    void PicoIPv6State::compress(uip_lladdr_t* ll_dest_addr){
      //set the variables that are needed for the compression
      uip_length = this->uip_len;
      uip_buf = uip_buffer;
      
      //compress
      output(ll_dest_addr);
      
      send_at_mac(ll_dest_addr);
      
      uip_length = 0;      
    }

    PicoIPv6State::PicoIPv6State(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address){
      //PRINTF("CONSTRUCTOR RA STATE");
      uip_len = 0;  
      uip_buf = buffer;    
      this->mac = mac;
      this->uip_buffer = buffer;
      this->own_ll_address = own_ll_address;
      this->gateway_ll_address = gateway_ll_address;
      this->gateway_ip_address = gateway_ip_address;
      this->udp_input_callback = NULL;
      //uip_create_linklocal_prefix(&own_ll_ip_address);
      //uip_ds6_set_addr_iid(&own_ll_ip_address, own_ll_address);
      uip_ip6addr(&own_ip_address, 0, 0, 0, 0, 0, 0, 0, 0);
      uip_ds6_set_addr_iid(&own_ip_address, own_ll_address);
      sicslowpan_init(own_ll_address);
      dodag_instance_id = -1; //not yet initialized

    }
    
    PicoIPv6State::~PicoIPv6State(){}
      
    
    MACLayer* PicoIPv6State::getMacCommunicator(){
      return mac;
    }
    
    uint8_t PicoIPv6State::getInstanceId(){
      return dodag_instance_id;
    }
    
    void PicoIPv6State::changeState(ConnectionState newState){
      this->currentState = newState; 
    }
    
    ConnectionState PicoIPv6State::getState(){
      return currentState; 
    }
    
    
    FrameReceptionResult PicoIPv6State::receive_mac(){
        
          if (mac->receive(&src_resp, &dst_resp, uip_6lp_buf.u8, &uip_6lp_len)){

        	  /*
        	  // DEBUGGING
        	  // Test for dst_resp_ptr proper modification if Broadcast or Unicast destination.
        	  Serial.println("");
        	  Serial.println("[RECEIVE] receive_mac destination MAC ADDRESS (Myself or Broadcast): 0x");
				  for (int i=0; i<8; ++i){ Serial.println( dst_resp.addr[i], HEX); }
        	   */

              //Now decompress (sicslowpan)
              input(&src_resp, &dst_resp);
              
              this->uip_len = uip_length;//we assign the decompressed length of the ipv6 packet
              
                      PRINTF("DECOMPRESSED");
                      
                      PRINTHEX(uip_len);
                      
                      for(int i=0; i<uip_len; ++i){
                         PRINTHEX(uip_buffer[i]);
                      }
              
              
              return RECEIVED;
          }else
            return NOTHING;
      
    }

    /*---------------------------------*/
    
    int
    PicoIPv6State::uip_rpl_input(void)
    {
      PRINTF("Received an RPL control message\n");
      switch(UIP_ICMP_BUF->icode) {
      case RPL_CODE_DIO:
        process_input();  
        lastReceived = DIO;    
        /* DAG Information Object */
        PRINTF("RPL: Received a DIO");
        break;
      case RPL_CODE_DIS:
        //dis_input();
        PRINTF("RPL: Received a DIS");
        break;
      case RPL_CODE_DAO:
        //dao_input();
        PRINTF("RPL: Received a DAO");
        break;
      case RPL_CODE_DAO_ACK:
        //dao_ack_input();
        process_input();
        lastReceived = DAO_ACK;
        PRINTF("RPL: Received a DAO ACK");
        break;
      default:
        PRINTF("RPL: received an unknown ICMP6 code");
        break;
      }
    
      uip_len = 0;
  
      return UIP_ICMP_BUF->icode;
    }
    
    /*------------------*/
    
        
#define DROP drop(); \
             return lastReceived
    
    /*---------------------------------*/
    
    ReceivedPacketType PicoIPv6State::udp_input(){
        uint16_t udp_data_len;
        uint8_t* udp_data;
        
        PRINTF("Receiving UDP packet");
       
        /* UDP processing is really just a hack. We don't do anything to the
           UDP/IP headers, but let the UDP application do all the hard
           work. If the application sets uip_slen, it has a packet to
           send. */
           
      #if UIP_UDP_CHECKSUMS
        if(UIP_UDPIP_BUF->udpchksum != 0 && uip_udpchksum() != 0xffff) {
          PRINTF("udp: bad checksum");
          DROP;
        }
      #endif /* UIP_UDP_CHECKSUMS */
      
      udp_data = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];
      udp_data_len = uip_len - UIP_IPUDPH_LEN;
      
      PRINTHEX(udp_data_len);
      
        /* Make sure that the UDP destination port number is not zero. */
        if(UIP_UDPIP_BUF->destport == 0) {
          PRINTF("udp: zero port.");
          DROP;
        }
      
        //UIP_UDP_APPCALL();
        this->udp_input_callback(udp_data, udp_data_len);
        
        lastReceived = UDP;
        
        return lastReceived;
    }
    
    
    /*------------------------------------------------------------------*/
    /* create a llao */ 
    void PicoIPv6State::create_llao(uint8_t *llao, uint8_t type) {
      llao[UIP_ND6_OPT_TYPE_OFFSET] = type;
      llao[UIP_ND6_OPT_LEN_OFFSET] = UIP_ND6_OPT_LLAO_LEN >> 3;
      memcpy(&llao[UIP_ND6_OPT_DATA_OFFSET], own_ll_address, UIP_LLADDR_LEN);
      /* padding on some */
      memset(&llao[UIP_ND6_OPT_DATA_OFFSET + UIP_LLADDR_LEN], 0,
             UIP_ND6_OPT_LLAO_LEN - 2 - UIP_LLADDR_LEN);
    }

    /*------------------------------------------------------------------*/

      void PicoIPv6State::uip_nd6_ns_input(void)
      {
        PRINTF("Received NS");
        u8_t flags;
        bool gotoDiscard = true;
      
      #if UIP_CONF_IPV6_CHECKS
        if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) ||
           (uip_is_addr_mcast(&UIP_ND6_NS_BUF->tgtipaddr)) ||
           (UIP_ICMP_BUF->icode != 0)) {
          PRINTF("NS received is bad");
          goto discard;
        }
      #endif /* UIP_CONF_IPV6_CHECKS */
      
      uint8_t nd6_opt_offset;                     /** Offset from the end of the icmpv6 header to the option in uip_buf*/
      uint8_t *nd6_opt_llao;   /**  Pointer to llao option in uip_buf */
        /* Options processing */
        nd6_opt_llao = NULL;
        nd6_opt_offset = UIP_ND6_NS_LEN;
        while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
      #if UIP_CONF_IPV6_CHECKS
          if(UIP_ND6_OPT_HDR_BUF->len == 0) {
            PRINTF("NS received is bad");
            goto discard;
          }
      #endif /* UIP_CONF_IPV6_CHECKS */
          switch (UIP_ND6_OPT_HDR_BUF->type) {
          case UIP_ND6_OPT_SLLAO:
            nd6_opt_llao = &uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset];
      #if UIP_CONF_IPV6_CHECKS
            /* There must be NO option in a DAD NS */
            if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
              PRINTF("NS received is bad");
              goto discard;
            }
      #endif /*UIP_CONF_IPV6_CHECKS */
            break;
          default:
            PRINTF("ND option not supported in NS");
            break;
          }
          nd6_opt_offset += (UIP_ND6_OPT_HDR_BUF->len << 3);
        }
              
        //Is this (target address) my own ip address
        if (uip_is_addr_linklocal(&UIP_ND6_NS_BUF->tgtipaddr)){
           //Check if it is our link local address
           uip_create_linklocal_prefix(&own_ip_address);
           if(uip_ipaddr_cmp(&own_ip_address, &UIP_ND6_NS_BUF->tgtipaddr)){
             gotoDiscard = false;
           }
           //Go back to our real address
           uip_create_default_prefix(&own_ip_address);
        }else{
           if (uip_ipaddr_cmp(&own_ip_address, &UIP_ND6_NS_BUF->tgtipaddr))
             gotoDiscard = false;
        }
        if(!gotoDiscard) {
          if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
            /* DAD CASE */
      #if UIP_CONF_IPV6_CHECKS
            if(!uip_is_addr_solicited_node(&UIP_IP_BUF->destipaddr)) {
              PRINTF("NS received is bad");
              goto discard;
            }
      #endif /* UIP_CONF_IPV6_CHECKS */
            /*
              if(addr->state != ADDR_TENTATIVE) {
              uip_create_linklocal_allnodes_mcast(&UIP_IP_BUF->destipaddr);
              uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
              flags = UIP_ND6_NA_FLAG_OVERRIDE;
              goto create_na;
            } else {
              uip_ds6_dad_failed(addr);
              goto discard;
            }
            */
          }
      #if UIP_CONF_IPV6_CHECKS
          if(/*uip_ds6_is_my_addr(&UIP_IP_BUF->srcipaddr)*/uip_ipaddr_cmp(&own_ip_address, &UIP_IP_BUF->srcipaddr)) {
              /**
               * \NOTE do we do something here? we both are using the same address.
               * If we are doing dad, we could cancel it, though we should receive a
               * NA in response of DAD NS we sent, hence DAD will fail anyway. If we
               * were not doing DAD, it means there is a duplicate in the network!
               */
            PRINTF("NS received is bad");
            goto discard;
          }
      #endif /*UIP_CONF_IPV6_CHECKS */
      
          /* Address resolution case */
          if(uip_is_addr_solicited_node(&UIP_IP_BUF->destipaddr)) {
            uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
            uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
            flags = UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE;
            goto create_na;
          }
      
          /* NUD CASE */
          if(/*uip_ds6_addr_lookup(&UIP_IP_BUF->destipaddr) == addr*/uip_ipaddr_cmp(gateway_ip_address, &UIP_IP_BUF->destipaddr)) {
            uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
            uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
            flags = UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE;
            goto create_na;
          } else {
      #if UIP_CONF_IPV6_CHECKS
            PRINTF("NS received is bad");
            goto discard;
      #endif /* UIP_CONF_IPV6_CHECKS */
          }
        } else {
          goto discard;
        }
      
      
      create_na:
        UIP_IP_BUF->vtc = 0x60;
        UIP_IP_BUF->tcflow = 0;
        UIP_IP_BUF->flow = 0;
        UIP_IP_BUF->len[0] = 0;       /* length will not be more than 255 */
        UIP_IP_BUF->len[1] = UIP_ICMPH_LEN + UIP_ND6_NA_LEN + UIP_ND6_OPT_LLAO_LEN;
        UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
        UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;
      
        UIP_ICMP_BUF->type = ICMP6_NA;
        UIP_ICMP_BUF->icode = 0;
      
        UIP_ND6_NA_BUF->flagsreserved = flags;
        memcpy(&UIP_ND6_NA_BUF->tgtipaddr, &UIP_ND6_NS_BUF->tgtipaddr, sizeof(uip_ipaddr_t));
      
        create_llao(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_NA_LEN], UIP_ND6_OPT_TLLAO);
      
        UIP_ICMP_BUF->icmpchksum = 0;
        UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();
      
        uip_len =
          UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NA_LEN + UIP_ND6_OPT_LLAO_LEN;
      
        PRINTF("Sending NA");
        //READY TO SEND! WE SEND IT RIGHT NOW
        ip_send(&UIP_IP_BUF->destipaddr);
        return;
      
      discard:
        uip_len = 0;
        return;
      }
    
    /*---------------------------------*/
    
    ReceivedPacketType PicoIPv6State::icmp6_input(){
      /* This is IPv6 ICMPv6 processing code. */
      PRINTF("icmp6_input: length");

      #if UIP_CONF_IPV6_CHECKS
        /* Compute and check the ICMP header checksum */
        if(uip_icmp6chksum() != 0xffff) {
          UIP_STAT(++uip_stat.icmp.drop);
          UIP_STAT(++uip_stat.icmp.chkerr);
          UIP_LOG("icmpv6: bad checksum.");
          DROP;
        }
      #endif /*UIP_CONF_IPV6_CHECKS*/

      UIP_STAT(++uip_stat.icmp.recv);
    
      switch(UIP_ICMP_BUF->type) {
        
        case ICMP6_RPL:
          uip_rpl_input();
          break;
        
        case ICMP6_NS:
          uip_nd6_ns_input();
          break;
        case ICMP6_NA:
          DROP;
          break;
        case ICMP6_RS:
          DROP;
          break;
        case ICMP6_RA:
          DROP;
          break;  
        case ICMP6_ECHO_REQUEST:
          DROP;
          break;
        case ICMP6_ECHO_REPLY:
          DROP;
          break;
        default:
          PRINTF("Not expected icmp6 message type");
          UIP_STAT(++uip_stat.icmp.drop);
          UIP_STAT(++uip_stat.icmp.typeerr);
          UIP_LOG("icmp6: unknown ICMP message.");
          DROP;
          break;
      }
      
      return lastReceived;
      
    }
    
    /*---------------------------------*/
    
    void PicoIPv6State::drop(){
        uip_len = 0;
        /*uip_ext_len = 0;
        uip_ext_bitmap = 0;
        uip_flags = 0;*/
        lastReceived = NOT_EXPECTED_OR_ERROR;
    }
    
    /*--------------------------------*/
    
    ReceivedPacketType PicoIPv6State::receive_ipv6(){
      if (uip_len <= 0){
        PRINTF("NO IP LENGTH");
        DROP;
      }
        
      /* Start of IP input header processing code. */
         
        /* Check validity of the IP header. */
        if((UIP_IP_BUF->vtc & 0xf0) != 0x60)  { /* IP version and header length. */
          UIP_STAT(++uip_stat.ip.drop);
          UIP_STAT(++uip_stat.ip.vhlerr);
          UIP_LOG("ipv6: invalid version.");
          DROP;
        }
        /*
         * Check the size of the packet. If the size reported to us in
         * uip_len is smaller the size reported in the IP header, we assume
         * that the packet has been corrupted in transit. If the size of
         * uip_len is larger than the size reported in the IP packet header,
         * the packet has been padded and we set uip_len to the correct
         * value..
         */
         
        if((UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] <= uip_len) {
          uip_len = (UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + UIP_IPH_LEN;
          /*
           * The length reported in the IPv6 header is the
           * length of the payload that follows the
           * header. However, uIP uses the uip_len variable
           * for holding the size of the entire packet,
           * including the IP header. For IPv4 this is not a
           * problem as the length field in the IPv4 header
           * contains the length of the entire packet. But
           * for IPv6 we need to add the size of the IPv6
           * header (40 bytes).
           */
        } else {
          UIP_LOG("ip: packet shorter than reported in IP header.");
          DROP;
        }
        
        PRINTF("IPv6 packet received from ");
        PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
        PRINTF(" to ");
        PRINT6ADDR(&UIP_IP_BUF->destipaddr);
        PRINTF("\n");
      
        if(uip_is_addr_mcast(&UIP_IP_BUF->srcipaddr)){
          UIP_STAT(++uip_stat.ip.drop);
          PRINTF("Dropping packet, src is mcast\n");
          DROP;
        }
        
        /* TODO: SEE WHICH ADDRESS RECEIVES THE PACKETS AND COMPARE IF IT IS OURS
        if(!uip_ds6_is_my_addr(&UIP_IP_BUF->destipaddr)) {
          PRINTF("Dropping packet, not for me\n");
          UIP_STAT(++uip_stat.ip.drop);          
          DROP;
        }
        */
        
        uip_ipaddr_copy(&src_ip_address, &UIP_IP_BUF->srcipaddr);
        
        /*
         * Next header field processing. In IPv6, we can have extension headers,
         * they are processed here
         */
          switch(UIP_IP_BUF->proto){
            case UIP_PROTO_UDP:
              /* UDP, for both IPv4 and IPv6 */
              udp_input();
              break;
            case UIP_PROTO_ICMP6:
              /* ICMPv6 */
              icmp6_input();
              break;
            default:
              DROP;
              break;
          }
                 
      uip_len = 0;
      
      return lastReceived;

    }
    
    void PicoIPv6State::uip_lladdr_copy(uip_lladdr_t *dest, const uip_lladdr_t *src, uint8_t addr_size)
    {
      u8_t i;
      for(i = 0; i < addr_size; i++) {
        dest->addr[i] = src->addr[i];
      }
    }
    
    bool PicoIPv6State::receive(){
      if (receive_mac() == RECEIVED){ //GOT SOMETHING AT MAC LEVEL
        PRINTF("RECEIVED AT MAC LEVEL");
        if (receive_ipv6() != NOT_EXPECTED_OR_ERROR){ //IF WE RECEIVED SOMETHING THAT WE COULD EXPECT, RECEPTION IS SUCCESSFUL
              PRINTF("RECEIVED STH");
              return true;
        }
      }
      return false;
    }
    
    void PicoIPv6State::setBuffer(uint8_t* buffer){
		this->uip_buffer = buffer;
    }
    
    uip_ip6addr_t* PicoIPv6State::getLastSenderIPAddress(){
		return &src_ip_address;
    }

    u16_t PicoIPv6State::getLastSenderUDPSourcePort(){
    	return UIP_NTOHS(UIP_UDPIP_BUF->srcport);
	}

	uint16_t PicoIPv6State::getUdpDataLength(){
		//  uip_len has value 0 does not work, should be fixed by :
		//		uip_len = (UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + UIP_IPH_LEN;
		//		and then return uip_len - UIP_IPUDPH_LEN;

		return UIP_NTOHS(UIP_UDPIP_BUF->udplen) - UIP_UDPH_LEN; // Works OK.
	}

	u16_t PicoIPv6State::getUdpData(char * buffer) {
		//copy UDP payload
		u16_t udp_data_len = getUdpDataLength();
		memcpy(buffer, &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN] , udp_data_len); //&uip_buffer[UIP_IPUDPH_LEN]
		return udp_data_len;
	}

    ReceivedPacketType PicoIPv6State::getLastReceivedPacketType(){
      return lastReceived; 
    }
