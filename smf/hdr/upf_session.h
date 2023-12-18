#ifndef UPF_SESSION_H
#define UPF_SESSION_H

int
get_pdr_and_far_for_session_create(uint32_t  ue_ipv4_addr,
                                   uint32_t  upf_n3_iface_v4_addr,
                                   uint32_t *session_idx,
                                   uint32_t *ul_teid);

int
get_pdr_and_far_for_session_modify(uint32_t  ue_ipv4_addr,
                                   uint32_t  latest_gnb_n3_iface_v4_addr,
                                   uint32_t  teid_rcvd_from_gnb,
                                   uint32_t *session_idx);

int
send_session_create_msg_to_upf(uint32_t      ue_ipv4_addr,
                               data_64bit_t  imsi,
                               uint32_t      upf_n3_iface_v4_addr,
                               uint32_t     *ul_teid,
                               uint8_t       debug_flag);

int
send_session_modify_msg_to_upf(uint32_t     ue_ipv4_addr,
                               data_64bit_t imsi,
                               uint32_t     latest_gnb_n3_iface_v4_addr,
                               uint32_t     teid_rcvd_from_gnb,
                               uint8_t      debug_flag);

#endif
