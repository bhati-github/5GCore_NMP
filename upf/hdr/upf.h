#ifndef UPF_H
#define UPF_H

#define MSG_BUFFER_LEN     2048

struct upf_config {
    uint8_t    debug_switch;

    uint16_t   my_id;
    uint16_t   amf_id;

    ip_addr_t  upf_n2_addr;
    ip_addr_t  amf_n2_addr;

    int        pkt_delay;

    int                 upf_n2_socket_id;
    struct sockaddr_in  amf_n2_sockaddr;
} __attribute__((packed));
typedef struct upf_config   upf_config_t;

extern upf_config_t  g__upf_config;
extern uint8_t  g__n2_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__n2_rcvd_msg_buffer[MSG_BUFFER_LEN];

#endif
