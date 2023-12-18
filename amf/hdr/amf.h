#ifndef AMF_H
#define AMF_H

#define MSG_BUFFER_LEN     2048
#define MAX_GNB_COUNT      1024

typedef struct {
    uint16_t            gnb_id;
    ip_addr_t           gnb_n1_n2_addr;
    ip_addr_t           gnb_n3_addr;
    struct sockaddr_in  gnb_n1_n2_sockaddr;
} gnb_data_t;

typedef struct {
    // used for making gtp-u tunnel in uplink direction
    uint32_t upf_n3_addr;
    uint32_t upf_n3_teid;

    // used for making gtp-u tunnel in dnlink direction
    uint32_t gnb_n3_addr;
    uint32_t gnb_n3_teid;
} smf_session_data_t;

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
    
    int        pkt_delay;	

    uint16_t    gnb_count;
    gnb_data_t  gnb_data[MAX_GNB_COUNT];

    smf_session_data_t smf_sessions[MAX_USERS];

} __attribute__((packed));
typedef struct amf_config   amf_config_t;

extern amf_config_t  g__amf_config;


extern uint8_t  g__n1_n2_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n1_n2_rcvd_msg_buffer[MSG_BUFFER_LEN];

extern uint8_t  g__Namf_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__Namf_rcvd_msg_buffer[MSG_BUFFER_LEN];

#endif
