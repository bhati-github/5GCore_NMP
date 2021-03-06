#ifndef UPF_SESSION_H
#define UPF_SESSION_H

int
get_pdr_and_far_for_session_create(uint32_t  ue_ipv4_addr,
                                   uint32_t  enb_v4_addr,
                                   uint32_t  upf_v4_addr,
                                   uint32_t *session_idx,
                                   uint32_t *ul_teid,
                                   uint32_t *dl_teid);

int
get_pdr_and_far_for_session_modify(uint32_t  ue_ipv4_addr,
                                   uint32_t  new_enb_v4_addr,
                                   uint32_t  upf_v4_addr,
                                   uint32_t *session_idx);

int
send_session_create_msg_to_upf(uint32_t      ue_ipv4_addr,
                               data_64bit_t  imsi,
                               uint32_t      enb_v4_addr,
                               uint32_t      upf_v4_addr,
                               uint32_t     *ul_teid,
                               uint32_t     *dl_teid,
                               uint8_t       debug_flag);

int
send_session_modify_msg_to_upf(uint32_t     ue_ipv4_addr,
                               data_64bit_t imsi,
                               uint32_t     new_enb_v4_addr,
                               uint32_t     upf_v4_addr,
                               uint8_t      debug_flag);



int
nmp_add_item_group__n3_pdr(uint8_t *ptr,
                           uint32_t session_index);

int
nmp_add_item_group__n6_pdr(uint8_t *ptr,
                           uint32_t session_index);

int
nmp_add_item_group__n3_far(uint8_t *ptr,
                           uint32_t session_index);

int
nmp_add_item_group__n6_far(uint8_t *ptr,
                           uint32_t session_index);


#endif
