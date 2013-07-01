
/* RPL message types */
#define RPL_CODE_DIS                   0x00   /* DAG Information Solicitation */
#define RPL_CODE_DIO                   0x01   /* DAG Information Option */
#define RPL_CODE_DAO                   0x02   /* Destination Advertisement Option */
#define RPL_CODE_DAO_ACK               0x03   /* DAO acknowledgment */

/* RPL control message options. */
#define RPL_OPTION_PAD1              	0
#define RPL_OPTION_PADN              	1
#define RPL_OPTION_DAG_METRIC_CONTAINER	2
#define RPL_OPTION_ROUTE_INFO        	3
#define RPL_OPTION_DAG_CONF          	4
#define RPL_OPTION_TARGET            	5
#define RPL_OPTION_TRANSIT           	6
#define RPL_OPTION_SOLICITED_INFO    	7
#define RPL_OPTION_PREFIX_INFO       	8
#define RPL_OPTION_TARGET_DESC          9

/* Default route lifetime as a multiple of the lifetime unit. */
#define RPL_DEFAULT_LIFETIME        0xff

#define ICMP6_RPL                       155

#define RPL_DAO_K_FLAG                   0x80 /* DAO ACK requested */

#define HOP_LIMIT                       0x40

typedef uint8_t rpl_lifetime_t;
