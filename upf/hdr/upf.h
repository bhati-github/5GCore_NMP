#ifndef UPF_H
#define UPF_H

#define MSG_BUFFER_LEN     2048

struct upf_config {
    uint8_t    debug_switch;

    uint16_t   my_id;
    uint16_t   amf_id;

    ip_addr_t  amf_n4_addr;
    ip_addr_t  upf_n4_addr;

    int        pkt_delay;

    int                 upf_n4_socket_id;
    struct sockaddr_in  amf_n4_sockaddr;
} __attribute__((packed));
typedef struct upf_config   upf_config_t;

extern upf_config_t  g__upf_config;
extern uint8_t  g__n4_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n4_rcvd_msg_buffer[MSG_BUFFER_LEN];

#endif
