
/*---ICMP CONSTANTS---*/
/** \name ICMPv6 message types */
/** @{ */
#define ICMP6_DST_UNREACH                 1	/**< dest unreachable */
#define ICMP6_PACKET_TOO_BIG	            2	/**< packet too big */
#define ICMP6_TIME_EXCEEDED	            3	/**< time exceeded */
#define ICMP6_PARAM_PROB	               4	/**< ip6 header bad */
#define ICMP6_ECHO_REQUEST              128  /**< Echo request */
#define ICMP6_ECHO_REPLY                129  /**< Echo reply */

#define ICMP6_RS                        133  /**< Router Solicitation */
#define ICMP6_RA                        134  /**< Router Advertisement */
#define ICMP6_NS                        135  /**< Neighbor Solicitation */
#define ICMP6_NA                        136  /**< Neighbor advertisement */
#define ICMP6_REDIRECT                  137  /**< Redirect */

#define ICMP6_RPL                       155  /**< RPL */
/** @} */


/** \name ICMPv6 Destination Unreachable message codes*/
/** @{ */
#define ICMP6_DST_UNREACH_NOROUTE         0 /**< no route to destination */
#define ICMP6_DST_UNREACH_ADMIN	         1 /**< administratively prohibited */
#define ICMP6_DST_UNREACH_NOTNEIGHBOR     2 /**< not a neighbor(obsolete) */
#define ICMP6_DST_UNREACH_BEYONDSCOPE     2 /**< beyond scope of source address */
#define ICMP6_DST_UNREACH_ADDR	         3 /**< address unreachable */
#define ICMP6_DST_UNREACH_NOPORT          4 /**< port unreachable */
/** @} */

/** \name ICMPv6 Time Exceeded message codes*/
/** @{ */
#define ICMP6_TIME_EXCEED_TRANSIT         0 /**< ttl==0 in transit */
#define ICMP6_TIME_EXCEED_REASSEMBLY      1 /**< ttl==0 in reass */
/** @} */

/** \name ICMPv6 Parameter Problem message codes*/
/** @{ */
#define ICMP6_PARAMPROB_HEADER            0 /**< erroneous header field */
#define ICMP6_PARAMPROB_NEXTHEADER        1 /**< unrecognized next header */
#define ICMP6_PARAMPROB_OPTION            2 /**< unrecognized option */
/** @} */

/** \brief Echo Request constant part length */
#define UIP_ICMP6_ECHO_REQUEST_LEN        4

/** \brief ICMPv6 Error message constant part length */
#define UIP_ICMP6_ERROR_LEN               4

/*--ICPMP CONST END--*/
