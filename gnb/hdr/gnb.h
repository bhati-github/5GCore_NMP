#ifndef GNB_H
#define GNB_H

#define MSG_BUFFER_LEN     2048
#define GNB_DNLINK_TEID_BASE 0x100 

typedef struct {
    data_64bit_t  imsi;

    // uplink gtp-u packet will have these tunnel parameters.
    uint32_t upf_n3_addr;
    uint32_t upf_n3_teid;

    // dnlink gtp-u packet will have these tunnel parameters.
    uint32_t gnb_n3_addr;
    uint32_t gnb_n3_teid;

} ue_session_data_t;

struct gnb_config {
    uint8_t    debug_switch;

    uint16_t   my_id;
    uint16_t   amf_id;

    ip_addr_t  my_n1_n2_addr;
    ip_addr_t  my_n3_addr;
    
    ip_addr_t  amf_n1_n2_addr;

    int        pkt_delay;

    data_64bit_t         ue_imsi_base;
    ue_session_data_t    ue_session_data[MAX_USERS];

    int                  my_n1_n2_socket_id;
    struct sockaddr_in   amf_n1_n2_sockaddr;
} __attribute__((packed));

typedef struct gnb_config   gnb_config_t;

extern gnb_config_t  g__gnb_config;

extern uint8_t  g__n1_n2_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n1_n2_rcvd_msg_buffer[MSG_BUFFER_LEN];

#endif
