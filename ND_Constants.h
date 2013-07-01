/**@{  Pointers to messages just after icmp header */
#define UIP_ND6_NS_BUF            ((uip_nd6_ns *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_ND6_NA_BUF            ((uip_nd6_na *)&uip_buf[uip_l2_l3_icmp_hdr_len])
/** @} */
/** Pointer to ND option */
#define UIP_ND6_OPT_HDR_BUF  ((uip_nd6_opt_hdr *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#define UIP_ND6_OPT_PREFIX_BUF ((uip_nd6_opt_prefix_info *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#define UIP_ND6_OPT_MTU_BUF ((uip_nd6_opt_mtu *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
/** @} */


#define uip_ext_len 0

/** \name ND6 option types */
/** @{ */
#define UIP_ND6_OPT_SLLAO               1
#define UIP_ND6_OPT_TLLAO               2
#define UIP_ND6_OPT_PREFIX_INFO         3
#define UIP_ND6_OPT_REDIRECTED_HDR      4
#define UIP_ND6_OPT_MTU                 5
/** @} */

/** \name ND6 message length (excluding options) */
/** @{ */
#define UIP_ND6_NA_LEN                  20  
#define UIP_ND6_NS_LEN                  20 

/** \name Neighbor Advertisement flags masks */
/** @{ */
#define UIP_ND6_NA_FLAG_ROUTER          0x80
#define UIP_ND6_NA_FLAG_SOLICITED       0x40
#define UIP_ND6_NA_FLAG_OVERRIDE        0x20
#define UIP_ND6_RA_FLAG_ONLINK          0x80 
#define UIP_ND6_RA_FLAG_AUTONOMOUS      0x40 
/** @} */

/* If the interface is 802.15.4. For now we use only long addresses */
#define UIP_ND6_OPT_SHORT_LLAO_LEN     8
#define UIP_ND6_OPT_LONG_LLAO_LEN      16
/** \brief length of a ND6 LLAO option for 802.15.4 */
#define UIP_ND6_OPT_LLAO_LEN UIP_ND6_OPT_LONG_LLAO_LEN 

/** \name ND6 option types */
/** @{ */
#define UIP_ND6_OPT_TYPE_OFFSET         0
#define UIP_ND6_OPT_LEN_OFFSET          1
#define UIP_ND6_OPT_DATA_OFFSET         2

#define UIP_ND6_HOP_LIMIT               255


/**
 * \name ND Option structures
 * @{
 */

/** \brief ND option header */
typedef struct uip_nd6_opt_hdr {
  uint8_t type;
  uint8_t len;
} uip_nd6_opt_hdr;

/**
 * \name ND message structures
 * @{
 */

/** 
 * \brief A neighbor solicitation constant part
 * 
 * Possible option is: SLLAO
 */
typedef struct uip_nd6_ns {
  uint32_t reserved;
  uip_ipaddr_t tgtipaddr;
} uip_nd6_ns;

/**
 * \brief A neighbor advertisement constant part.
 * 
 * Possible option is: TLLAO
 */ 
typedef struct uip_nd6_na {
  uint8_t flagsreserved;
  uint8_t reserved[3];
  uip_ipaddr_t tgtipaddr;
} uip_nd6_na;
