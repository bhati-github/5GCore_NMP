#ifndef AMF_H
#define AMF_H

#define MSG_BUFFER_LEN     2048
#define MAX_GNB_COUNT      1024

#define MAX_UPF_SESSIONS   65535
#define MAX_PDR 8
#define MAX_FAR 8
#define MAX_BAR 8
#define MAX_QER 8

#define N3_PDR_BASE 30
#define N6_PDR_BASE 60
#define N9_PDR_BASE 90

#define N3_FAR_BASE 300
#define N6_FAR_BASE 600
#define N9_FAR_BASE 900

#define QER_BASE    100
#define BAR_BASE    150

#define N3_UPLINK_TEID_BASE   100000
#define N3_DNLINK_TEID_BASE   200000

typedef struct {
    uint16_t            gnb_id;
    ip_addr_t           gnb_n1_n2_addr;
    ip_addr_t           gnb_n3_addr;
    struct sockaddr_in  gnb_n1_n2_sockaddr;
} gnb_data_t;

struct f_teid {
    uint32_t     teid;
    ip_addr_t    ip_addr;
} __attribute__((packed));
typedef struct f_teid   f_teid_t;


//////////////////////////////////
#define INTERFACE_TYPE_IS_N3  0
#define INTERFACE_TYPE_IS_N6  1
#define INTERFACE_TYPE_IS_N9  2 // TBD: check this value
struct pdi {
    uint8_t     src_interface;
    f_teid_t    f_teid;
    ip_addr_t   ue_ip_addr;
} __attribute__((packed));
typedef struct pdi   pdi_t;

struct forward_param {
    uint8_t    dst_interface;
    char       network_instance[128]; // network-instance name
    f_teid_t   f_teid;
} __attribute__((packed));
typedef struct forward_param  forward_param_t;

#define PDR_ACTION__OUTER_HDR_REMOVAL  1
struct pdr {
    uint16_t  rule_id;
    uint16_t  precedence;
    pdi_t     pdi;
    uint8_t   action; // uplink pdr action = 0x01 (outer header removal, gtp-u/udp/ip)
    uint16_t  far_id;
} __attribute__((packed));
typedef struct pdr   pdr_t;

#define FAR_ACTION_FLAG__DUPLICATE  0x10  // 4th bit
#define FAR_ACTION_FLAG__NOTIFY_CP  0x08  // 3rd bit
#define FAR_ACTION_FLAG__BUFFER     0x04  // 2nd bit
#define FAR_ACTION_FLAG__FORWARD    0x02  // 1st bit
#define FAR_ACTION_FLAG__DROP       0x01  // 0th bit
struct far {
    uint16_t         far_id;
    uint8_t          action_flags;
    forward_param_t  forward_param;
    uint8_t          bar_id;
} __attribute__((packed));
typedef struct far   far_t;

struct bar {
    uint16_t   bar_id;
} __attribute__((packed));
typedef struct bar  bar_t;

struct qer {
    uint16_t   rule_id;

    // 0 = gate is open, 1 = gate is closed (traffic blocked)
    uint8_t   ul_gate_status;
    uint8_t   dl_gate_status;

    uint32_t  ul_mbr; // Uplink maximum bitrate
    uint32_t  ul_gbr; // Uplink gauranteed bitrate

    uint32_t  dl_mbr; // Dnlink maximum bitrate
    uint32_t  dl_gbr; // Dnlink guaranteed bitrate
} __attribute__((packed));
typedef struct qer  qer_t;


struct session_info {
    uint32_t      ue_ipv4_addr;
    data_64bit_t  imsi;
    pdr_t         n3_pdr;
    pdr_t         n6_pdr;
    far_t         n3_far;
    far_t         n6_far;
} __attribute__((packed));
typedef struct session_info   session_info_t;


struct amf_config {
    uint8_t    debug_switch;

    uint16_t   my_id;
    uint16_t   nrf_id;
    uint16_t   smf_id;
    uint16_t   upf_id;

    ip_addr_t  my_n1_n2_addr;
    int        my_n1_n2_socket_id;
    
    ip_addr_t  my_Namf_addr;
    int        my_Namf_socket_id;
    
    ip_addr_t           Nnrf_addr;
    struct sockaddr_in  Nnrf_sockaddr;
    
    ip_addr_t           Nsmf_addr;
    struct sockaddr_in  Nsmf_sockaddr;
    
    ip_addr_t  smf_n4_addr;
    int        smf_n4_socket_id;

    int        pkt_delay;	

    uint16_t    gnb_count;
    gnb_data_t  gnb_data[MAX_GNB_COUNT];

    ip_addr_t           upf_n4_addr;
    struct sockaddr_in  upf_n4_sockaddr;
    ip_addr_t           upf_n3_addr;

    session_info_t      upf_session_data[MAX_UPF_SESSIONS];

} __attribute__((packed));
typedef struct amf_config   amf_config_t;

extern amf_config_t  g__amf_config;




extern uint8_t  g__n1_n2_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n1_n2_rcvd_msg_buffer[MSG_BUFFER_LEN];

extern uint8_t  g__n4_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n4_rcvd_msg_buffer[MSG_BUFFER_LEN];

extern uint8_t  g__Namf_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__Namf_rcvd_msg_buffer[MSG_BUFFER_LEN];

extern uint32_t g__ue_ipv4_addr_subnet;
extern uint32_t g__ue_ipv4_addr_mask;
extern uint32_t g__ue_ipv4_addr_base;

#endif
