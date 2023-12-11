#ifndef NRF_H
#define NRF_H

#define MSG_BUFFER_LEN     2048

struct nrf_config {
    uint8_t    debug_switch;

    uint16_t   my_id;
    uint16_t   amf_id;
    uint16_t   smf_id;

    ip_addr_t  Nnrf_addr;
    ip_addr_t  Namf_addr;
    ip_addr_t  Nsmf_addr;

    int        pkt_delay;

    int                 Nnrf_socket_id;
    struct sockaddr_in  Namf_sockaddr;
    struct sockaddr_in  Nsmf_sockaddr;

    uint8_t  amf_service_info_is_registered;
    uint8_t  smf_service_info_is_registered;

    uint8_t  amf_service_info[MSG_BUFFER_LEN];
    uint8_t  smf_service_info[MSG_BUFFER_LEN];
} __attribute__((packed));

typedef struct nrf_config   nrf_config_t;

extern nrf_config_t  g__nrf_config;

extern uint8_t  g__Nnrf_send_msg_buffer[MSG_BUFFER_LEN];
extern uint8_t  g__Nnrf_rcvd_msg_buffer[MSG_BUFFER_LEN];

#endif
