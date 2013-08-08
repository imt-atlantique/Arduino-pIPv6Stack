
#ifndef __SUPPRASTATE_H__
#define __SUPPRASTATE_H__

extern "C" {
    #include <inttypes.h>
    #include "conf.h"
	#include "sicsLowPan.h"
}   

#include <Arduino.h>
#include "MACLayer.h"
#include "RPL_Constants.h"
#include "ICMP_Constants.h"
#include "ND_Constants.h"

/*-----DEBUGGING-----*/
#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(s) arduino_debug(s)//printf(__VA_ARGS__)
#define PRINTHEX(s) arduino_debug_hex(s)
#define PRINT6ADDR(addr) //PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) //PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",lladdr->addr[0], lladdr->addr[1], lladdr->addr[2], lladdr->addr[3],lladdr->addr[4], lladdr->addr[5])
#else
#define PRINTF(s)
#define PRINTHEX(s)
#define PRINT6ADDR(addr)
#endif

#define UIP_LOGGING 1
#if UIP_LOGGING
#include <stdio.h>
#define UIP_LOG(m) arduino_debug(m);
#else
#define UIP_LOG(m)
#endif
/*-----DEBUGGING END-----*/

#define UIP_STATISTICS 0
#if UIP_STATISTICS == 1
extern struct uip_stats uip_stat;
#define UIP_STAT(s) s
#else
#define UIP_STAT(s)
#endif /* UIP_STATISTICS == 1 */

/*-----POINTER REDEFINITION-----*/
#define UIP_IP_BUF                          ((struct uip_ip_hdr *)uip_buffer)
#define UIP_ICMP_BUF     ((struct uip_icmp_hdr *)&uip_buffer[uip_l3_hdr_len])
#define UIP_ICMP_PAYLOAD ((unsigned char *)&uip_buffer[uip_l3_icmp_hdr_len])
#define UIP_UDPIP_BUF ((struct uip_udpip_hdr *)uip_buffer)
/*-----POINTER REDEFINITION END-----*/

#define UIP_LINK_MTU 1280 //Too high.. should change for a more realistic value

/**
 * Toggles if UDP checksums should be used or not.
 *
 * \note Support for UDP checksums is currently not included in uIP,
 * so this option has no function.
 *
 * \hideinitializer
 */
#ifdef UIP_CONF_UDP_CHECKSUMS
#define UIP_UDP_CHECKSUMS (UIP_CONF_UDP_CHECKSUMS)
#else
#define UIP_UDP_CHECKSUMS (UIP_CONF_IPV6)
#endif

/**
 * The sums below are quite used in ND. When used for uip_buf, we
 * include link layer length when used for uip_len, we do not, hence
 * we need values with and without LLH_LEN we do not use capital
 * letters as these values are variable
 */
#define uip_l2_l3_hdr_len (UIP_LLH_LEN + UIP_IPH_LEN + uip_ext_len)
#define uip_l2_l3_icmp_hdr_len (UIP_LLH_LEN + UIP_IPH_LEN + uip_ext_len + UIP_ICMPH_LEN)
#define uip_l3_hdr_len (UIP_IPH_LEN + uip_ext_len)
#define uip_l3_icmp_hdr_len (UIP_IPH_LEN + uip_ext_len + UIP_ICMPH_LEN)

/* The UDP and IP headers. */
struct uip_udpip_hdr {
  /* IPv6 header. */
  u8_t vtc,
    tcf;
  u16_t flow;
  u8_t len[2];
  u8_t proto, ttl;
  uip_ip6addr_t srcipaddr, destipaddr;  
  /* UDP header. */
  u16_t srcport,
    destport;
  u16_t udplen;
  u16_t udpchksum;
};

/**
 * Construct an IPv6 address from eight 16-bit words.
 *
 * This function constructs an IPv6 address.
 *
 * \hideinitializer
 */
#define uip_ip6addr(addr, addr0,addr1,addr2,addr3,addr4,addr5,addr6,addr7) do { \
    (addr)->u16[0] = UIP_HTONS(addr0);                                      \
    (addr)->u16[1] = UIP_HTONS(addr1);                                      \
    (addr)->u16[2] = UIP_HTONS(addr2);                                      \
    (addr)->u16[3] = UIP_HTONS(addr3);                                      \
    (addr)->u16[4] = UIP_HTONS(addr4);                                      \
    (addr)->u16[5] = UIP_HTONS(addr5);                                      \
    (addr)->u16[6] = UIP_HTONS(addr6);                                      \
    (addr)->u16[7] = UIP_HTONS(addr7);                                      \
  } while(0)


/** \brief set IP address a to the link local all-rpl nodes multicast address */
#define uip_create_linklocal_rplnodes_mcast(a) uip_ip6addr(a, 0xff02, 0, 0, 0, 0, 0, 0, 0x001a)

/**
 * \brief is address a multicast address, see RFC 3513
 * a is of type uip_ipaddr_t*
 * */
#define uip_is_addr_mcast(a)                    \
  (((a)->u8[0]) == 0xFF)
  
  
/**
 * \brief  is addr (a) a solicited node multicast address, see RFC3513
 *  a is of type uip_ipaddr_t*
 */
#define uip_is_addr_solicited_node(a)          \
  ((((a)->u8[0])  == 0xFF) &&                  \
   (((a)->u8[1])  == 0x02) &&                  \
   (((a)->u16[1]) == 0x00) &&                  \
   (((a)->u16[2]) == 0x00) &&                  \
   (((a)->u16[3]) == 0x00) &&                  \
   (((a)->u16[4]) == 0x00) &&                  \
   (((a)->u8[10]) == 0x00) &&                  \
   (((a)->u8[11]) == 0x01) &&                  \
   (((a)->u8[12]) == 0xFF))
  
#define uip_ipaddr_cmp(addr1, addr2) (memcmp(addr1, addr2, sizeof(uip_ip6addr_t)) == 0)  
  
#ifndef uip_ipaddr_copy
#define uip_ipaddr_copy(dest, src) (*(dest) = *(src))
#endif

#define uip_create_default_prefix(addr) do { \
    (addr)->u16[0] = 0;                        \  
    (addr)->u16[1] = 0;                        \
    (addr)->u16[2] = 0;                        \
    (addr)->u16[3] = 0;                        \
  } while(0)

#define uip_create_linklocal_prefix(addr) do { \
    (addr)->u16[0] = UIP_HTONS(0xfe80);            \
    (addr)->u16[1] = 0;                        \
    (addr)->u16[2] = 0;                        \
    (addr)->u16[3] = 0;                        \
  } while(0)

/**
 * \brief Checks whether the address a is link local.
 * a is of type uip_ipaddr_t
 */
#define uip_is_addr_linklocal(a)                 \
  ((a)->u8[0] == 0xfe &&                         \
   (a)->u8[1] == 0x80)

/*-----ENUMS-----*/
enum ConnectionState{
  DISCONNECTED, //means we have to send DIS
  WAITING_DIO, //means we sent DIS and we wait for the reception of a DIO
  DIO_RECEIVED, //means we received DIO and we have to send DAO
  WAITING_DAO_ACK, //means we sent DAO and we wait for the reception of a DAO ACK
  DAO_ACK_RECEIVED, //means we received the ACK of the DAO sent and that would be enough to consider connected
  CONNECTED //means we are already connected
};

enum FrameReceptionResult{
  TIMEOUT,
  RECEIVED,
  ERROR,
  NOTHING 
};

enum ReceivedPacketType{
  DIO,
  DAO_ACK,
  UDP,
  NOT_EXPECTED_OR_ERROR,
  NS
};
/*-----ENUMS END-----*/

class PicoIPv6State{
  
  private:
  
    ReceivedPacketType icmp6_input();
    
    ReceivedPacketType udp_input();
    
    int uip_rpl_input(void);
    
    void uip_nd6_ns_input(void);
    
    void create_llao(uint8_t *llao, uint8_t type);
    
    void drop();
  protected:
    MACLayer* mac;
    
    ReceivedPacketType lastReceived;
    
    uint8_t dodag_instance_id;	//instance id of the last DIO received
    
    uip_lladdr_t  dst_resp;		//				link layer destination address of the last received frame (own or broadcast)
    uip_lladdr_t  src_resp;		//				link layer source address of last received frame

    
    uip_ipaddr_t src_ip_address;  // ipv6 address of the source of the last received frame
    
    uip_lladdr_t* own_ll_address; // link layer address of this device (pointer to)
    uip_ip6addr_t own_ip_address; // this device's ip address
    
    uip_lladdr_t* gateway_ll_address;	// link layer address of the gateway corresponding to this device
    uip_ip6addr_t* gateway_ip_address;	// ip address of the gateway
    
    u8_t* uip_buffer;
    u16_t uip_len; // length of the data into the uip_buffer (this buffer contains the ip packet received or to be sent)
    
    void (* udp_input_callback)(uint8_t* udp_data, uint16_t length);//callback function called whenever an UDP packet is received. udp_data: pointer to the UDP payload received. length: size of the payload data
    
    
    u16_t uip_htons(u16_t val);
    
    u32_t uip_htonl(u32_t val);
    
    u16_t chksum(u16_t sum, const u8_t *data, u16_t len);
    
    u16_t uip_chksum(u16_t *data, u16_t len);
    
    u16_t uip_ipchksum(void);
    
    u16_t upper_layer_chksum(u8_t proto);    
    
    u16_t uip_udpchksum(void);
    
    u16_t uip_icmp6chksum(void);
        
    void icmp6_send(uip_ipaddr_t *dest, int type, int code, int payload_len);
    
    void ip_send(uip_ipaddr_t *dest);
    
    void compress(uip_lladdr_t* ll_dest_addr);
    
    void send_at_mac(uip_lladdr_t* ll_dest_addr);
    
    FrameReceptionResult receive_mac(/*int timeout*/);
    
    ReceivedPacketType receive_ipv6();
        
    ConnectionState currentState;
    
    virtual void process_input() = 0;
    
    void uip_lladdr_copy(uip_lladdr_t *dest, const uip_lladdr_t *src, uint8_t addr_size);
    
  public:    
      
    PicoIPv6State(MACLayer* mac, u8_t* buffer, uip_lladdr_t* own_ll_address, uip_lladdr_t* gateway_ll_address, uip_ip6addr_t* gateway_ip_address);
    
    virtual ~PicoIPv6State();
    
    virtual ConnectionState connect() = 0;  
  
    void setBuffer(uint8_t* buffer);  
        
    uip_lladdr_t* getLastSenderMacAddress();
    
    uip_ip6addr_t* getLastSenderIPAddress();
    

    u16_t getLastSenderUDPSourcePort();
	uint16_t getUdpDataLength();
	u16_t getUdpData(char * buffer);

    MACLayer* getMacCommunicator();
    
    uint8_t getInstanceId();
    
    void changeState(ConnectionState newState);
    
    ConnectionState getState();
    
    virtual bool has_timer_expired() = 0;
            
    void udp_send(uip_ipaddr_t *dest, u16_t src_port, u16_t dest_port, char* payload, u16_t payload_len);
    
    bool receive();
    
    ReceivedPacketType getLastReceivedPacketType();
};

#endif
