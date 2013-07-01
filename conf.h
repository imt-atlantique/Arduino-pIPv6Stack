#ifndef __CONF_H__
#define __CONF_H__

#include <inttypes.h>
#include <stdint.h>


typedef uint8_t   u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef  int32_t s32_t;

#ifndef UIP_LITTLE_ENDIAN
#define UIP_LITTLE_ENDIAN  3412
#endif /* UIP_LITTLE_ENDIAN */
#ifndef UIP_BIG_ENDIAN
#define UIP_BIG_ENDIAN     1234
#endif /* UIP_BIG_ENDIAN */

#define UIP_CONF_BYTE_ORDER      UIP_LITTLE_ENDIAN

#ifdef UIP_CONF_BYTE_ORDER
#define UIP_BYTE_ORDER     (UIP_CONF_BYTE_ORDER)
#else /* UIP_CONF_BYTE_ORDER */
#define UIP_BYTE_ORDER     (UIP_LITTLE_ENDIAN)
#endif /* UIP_CONF_BYTE_ORDER */


/**
 * Convert 16-bit quantity from host byte order to network byte order.
 *
 */
#   if UIP_BYTE_ORDER == UIP_BIG_ENDIAN
#      define UIP_HTONS(n) (n)
#      define UIP_HTONL(n) (n)
#   else /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */
#      define UIP_HTONS(n) (u16_t)((((u16_t) (n)) << 8) | (((u16_t) (n)) >> 8))
#      define UIP_HTONL(n) (((u32_t)UIP_HTONS(n) << 16) | UIP_HTONS((u32_t)(n) >> 16))
#   endif /* UIP_BYTE_ORDER == UIP_BIG_ENDIAN */

/**
 * Convert 16-bit quantity from network byte order to host byte order.
 */
#ifndef UIP_NTOHS
#define UIP_NTOHS UIP_HTONS
#endif


#define UIP_CONF_IPV6 1

#define UIP_CONF_UDP            1 
#define UIP_CONF_UDP_CONNS	1

#define UIP_CONF_BYTE_ORDER      UIP_LITTLE_ENDIAN
#define UIP_CONF_BUFFER_SIZE     420
#define UIP_CONF_TCP      0 
#define UIP_CONF_MAX_CONNECTIONS 3 
#define UIP_CONF_MAX_LISTENPORTS 1 
#define UIP_CONF_TCP_SPLIT       1
#define UIP_CONF_LOGGING         0
#define UIP_CONF_UDP_CHECKSUMS   1 

#if UIP_CONF_IPV6
#define UIP_CONF_IPV6_CHECKS    1 
#define UIP_CONF_IPV6_QUEUE_PKT 0 
#define UIP_CONF_IPV6_REASSEMBLY 0
#define UIP_CONF_NETIF_MAX_ADDRESSES  10 
#define UIP_CONF_ND6_MAX_PREFIXES     2
#define UIP_CONF_ND6_MAX_NEIGHBORS  3 
#define UIP_CONF_ND6_MAX_DEFROUTERS 1 
#define UIP_CONF_ICMP6           1
#endif /* UIP_CONF_ICMP6 */

#define SICSLOWPAN_CONF_FRAG 0

#define SICSLOWPAN_CONF_COMPRESSION_IPV6        0
#define SICSLOWPAN_CONF_COMPRESSION_HC1         1
#define SICSLOWPAN_CONF_COMPRESSION_HC06        2
#define SICSLOWPAN_CONF_COMPRESSION             2//SICSLOWPAN_COMPRESSION_HC06

/* Specify a minimum packet size for 6lowpan compression to be
   enabled. This is needed for ContikiMAC, which needs packets to be
   larger than a specified size, if no ContikiMAC header should be
   used. */
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD 64 // 0: Always Compress a Packet Using SICSLOWPAN

#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS 1

#define RPL_CONF_DAO_ACK 1

#define UIP_PROTO_ICMP  1
#define UIP_PROTO_TCP   6
#define UIP_PROTO_UDP   17
#define UIP_PROTO_ICMP6 58

#define UIP_IPH_LEN    40	/* Size of IPv6 header */
#define UIP_FRAGH_LEN  8

#define UIP_UDPH_LEN    8    /* Size of UDP header */
#define UIP_ICMPH_LEN   4    /* Size of ICMP header */
#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)    /* Size of IP + UDP header */
#define UIP_TCPIP_HLEN UIP_IPTCPH_LEN
#define UIP_IPICMPH_LEN (UIP_IPH_LEN + UIP_ICMPH_LEN) /* size of ICMP + IP header */
                                                         
 #define uip_l3_hdr_len (UIP_IPH_LEN)
#define uip_l3_icmp_hdr_len (UIP_IPH_LEN + UIP_ICMPH_LEN)

typedef union uip_ip6addr_t {
  u8_t  u8[16];			/* Initializer, must come first!!! */
  u16_t u16[8];
} uip_ip6addr_t;

typedef uip_ip6addr_t uip_ipaddr_t;

/*
 * In IPv6 the length of the L3 headers before the transport header is
 * not fixed, due to the possibility to include extension option headers
 * after the IP header. hence we split here L3 and L4 headers
 */
/* The IP header */
struct uip_ip_hdr {
  /* IPV6 header */
  u8_t vtc;
  u8_t tcflow;
  u16_t flow;
  u8_t len[2];
  u8_t proto, ttl;
  uip_ip6addr_t srcipaddr, destipaddr;
};


/* The ICMP headers. */
struct uip_icmp_hdr {
  u8_t type, icode;
  u16_t icmpchksum;
};


/* The UDP headers. */
struct uip_udp_hdr {
  u16_t srcport;
  u16_t destport;
  u16_t udplen;
  u16_t udpchksum;
};

typedef uint32_t clock_time_t;
#define CLOCK_CONF_SECOND 32 

#define UIP_802154_LONGADDR_LEN 8
#define UIP_802154_SHORTADDR_LEN 2

#define UIP_LLADDR_LEN UIP_802154_LONGADDR_LEN

typedef struct uip_802154_longaddr {
  u8_t addr[UIP_LLADDR_LEN];
} uip_802154_longaddr;

typedef uip_802154_longaddr uip_lladdr_t;

#define UIP_LLH_LEN 0 //no additional Link Layer headers

#define UIP_LLIPH_LEN (UIP_LLH_LEN + UIP_IPH_LEN)    /* size of L2
                                                        + IP header */

#define UIP_BUFSIZE  160 // The size of the uIP packet buffer.
#define UIP_UDP_PAYLOAD_MAX_BUFSIZE (UIP_BUFSIZE - UIP_IPUDPH_LEN) // The max size of the UDP datagram payload buffer. ( 160 - 48 )

#define CHAR_BIT 8

#endif
