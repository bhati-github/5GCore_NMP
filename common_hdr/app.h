#ifndef APP_H
#define APP_H

#define MAX_USERS  65535


typedef union {
    uint8_t  u8[4];
    uint16_t u16[2];
    uint32_t u32;
} data_32bit_t;

typedef union {
    uint8_t  u8[8];
    uint16_t u16[4];
    uint32_t u32[2];
    uint64_t u64;
} data_64bit_t;

typedef union {
    uint8_t   u8[16];
    uint16_t  u16[8];
    uint32_t  u32[4];
    uint64_t  u64[2];
} ipv6_addr_t;

struct ip_addr {
    uint8_t     ip_version;
    union {
        uint32_t    v4_addr;
        ipv6_addr_t v6_addr;
    } u;
} __attribute__((packed));
typedef struct ip_addr  ip_addr_t;

#define IP_VER_IS_V4                  4
#define IPV4_FLAG_IS_RF               4
#define IPV4_FLAG_IS_DF               2
#define IPV4_FLAG_IS_MF               1
#define IPV4_FRAG_OFFSET_MASK         0x1fff
#define IP_PROTOCOL_IS_ICMP           1
#define IP_PROTOCOL_IS_IP_IN_IP       4
#define IP_PROTOCOL_IS_TCP            6
#define IP_PROTOCOL_IS_UDP            17
#define IP_PROTOCOL_IS_IPV6_IN_IP     41
#define IP_PROTOCOL_IS_ESP            50
#define IPV4_HDR_SIZE                 20
typedef struct {
    uint8_t  version_hlen;
    uint8_t  tos;
    uint16_t length;
    uint16_t identifier;
    uint16_t flags_and_offset;
    uint8_t  time_to_live;
    uint8_t  ip_protocol;
    uint16_t checksum;
    uint32_t src_addr;
    uint32_t dst_addr;
} ipv4_hdr_t;

#define IP_VER_IS_V6                   6
#define IPV6_HDR_SIZE                  40
#define IPV6_NEXT_HDR_IS_TCP           6
#define IPV6_NEXT_HDR_IS_UDP           17
#define IPV6_NEXT_HDR_IS_FRAGMENT_HDR  44
#define IPV6_NEXT_HDR_IS_ICMPv6        58
typedef struct {
    uint32_t    version_tr_class_flow_label; // 4 bit, 8 bit and 20 bit
    uint16_t    length;
    uint8_t     next_hdr;
    uint8_t     hop_limit;
    ipv6_addr_t src_addr;
    ipv6_addr_t dst_addr;
} ipv6_hdr_t;

#define IPV6_FRAGMENT_HDR_SIZE  8
typedef struct {
    uint8_t   next_hdr;
    uint8_t   reserved;
    uint16_t  flags_and_offset;
    uint32_t  identifier;
} ipv6_fragment_hdr_t;

#define ICMP_HDR_SIZE  8
typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;
} icmp_hdr_t;

#define ICMP_V6_HDR_SIZE 24
#define ICMP_V6_HDR_TYPE_PING_REQUEST           128
#define ICMP_V6_HDR_TYPE_PING_REPLY             129
#define ICMP_V6_HDR_TYPE_NEIGHBOR_SOLICITATION  135
#define ICMP_V6_HDR_TYPE_NEIGHBOR_ADVERTISEMENT 136
typedef struct {
    uint8_t      type;
    uint8_t      code;
    uint16_t     checksum;
    uint32_t     flags;
    ipv6_addr_t  target_addr;
} icmp_v6_hdr_t;

#define UDP_HDR_SIZE   8
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} udp_hdr_t;

#define TCP_HDR_SIZE   20
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t hdr_len:8;
    uint16_t flags:8;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t padding;
} tcp_hdr_t;


////////////////////////////////////////
////////////////////////////////////////
#define GNB_ID_BASE        100        
#define AMF_ID_BASE        5100
#define NRF_ID_BASE        5200
#define SMF_ID_BASE        5300
#define UPF_ID_BASE        5400
#define UDP_PORT_IS_NMP    1208

#define NODE_TYPE__UE      1
#define NODE_TYPE__GNB     2
#define NODE_TYPE__AMF     3
#define NODE_TYPE__NRF     4
#define NODE_TYPE__SMF     5
#define NODE_TYPE__UPF     6

#define MSG_RESPONSE_CODE__OK      201
#define MSG_RESPONSE_CODE__NOT_OK  404

enum {
    MSG_TYPE__KEEPALIVE_REQ = 0,
    MSG_TYPE__KEEPALIVE_RESP = 1,

    MSG_TYPE__ALL_OK = 2,
    MSG_TYPE__NOT_OK = 3,

    MSG_TYPE__NG_SETUP_REQ = 11,
    MSG_TYPE__NG_SETUP_RESP = 12,

    MSG_TYPE__INITIAL_UE_MSG_REGISTRATION_REQ = 21,
    MSG_TYPE__DNLINK_NAS_TRANSPORT_AUTH_REQ = 22,
    MSG_TYPE__UPLINK_NAS_TRANSPORT_AUTH_RESP = 23,
    MSG_TYPE__DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT = 24,
    MSG_TYPE__UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE = 25,
    MSG_TYPE__UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ = 26,
    MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT = 27,
    MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REJECT = 28,
    MSG_TYPE__PDU_SESSION_RESOURCE_SETUP_RESP = 29,

    MSG_TYPE__NRF_SERVICE_REGISTRATION_REQ = 41,
    MSG_TYPE__NRF_SERVICE_REGISTRATION_RESP = 42,
    MSG_TYPE__NRF_SERVICE_DISCOVERY_REQ = 43,
    MSG_TYPE__NRF_SERVICE_DISCOVERY_RESP = 44,

    // AMF <--> SMF
    MSG_TYPE__SMF_SESSION_CREATE_REQ = 51,
    MSG_TYPE__SMF_SESSION_CREATE_RESP = 52,
    MSG_TYPE__SMF_SESSION_MODIFY_REQ = 53,
    MSG_TYPE__SMF_SESSION_MODIFY_RESP = 54,

    // SMF <--> UPF
    MSG_TYPE__UPF_SESSION_CREATE_REQ = 61,
    MSG_TYPE__UPF_SESSION_CREATE_RESP = 62,
    MSG_TYPE__UPF_SESSION_MODIFY_REQ = 63,
    MSG_TYPE__UPF_SESSION_MODIFY_RESP = 64,
    MSG_TYPE__UPF_SESSION_DELETE_REQ = 65,
    MSG_TYPE__UPF_SESSION_DELETE_RESP = 66,
};



#endif
