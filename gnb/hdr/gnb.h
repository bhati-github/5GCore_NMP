#ifndef GNB_H
#define GNB_H

#define MAX_USERS          65535
#define MSG_BUFFER_LEN     2048

struct gnb_config {
    uint8_t    debug_switch;

    uint16_t   my_id;
    uint16_t   amf_id;

    ip_addr_t  gnb_n1_addr;
    ip_addr_t  amf_n1_addr;

    ip_addr_t  gnb_n3_addr;

    int        pkt_delay;

    int                 gnb_n1_socket_id;
    struct sockaddr_in  amf_n1_sockaddr;
} __attribute__((packed));
typedef struct gnb_config   gnb_config_t;

extern gnb_config_t  g__gnb_config;
extern uint8_t  g__n1_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n1_rcvd_msg_buffer[MSG_BUFFER_LEN];

#endif
