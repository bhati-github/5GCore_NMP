#ifndef UE_ATTACH_H
#define UE_ATTACH_H

int
send_ue_attach_response_not_ok(nmp_msg_data_t *nmp_n1_rcvd_msg_data_ptr,
                               uint8_t         debug_flag);

int
send_ue_attach_response_ok(nmp_msg_data_t *nmp_n1_rcvd_msg_data_ptr,
                           uint32_t        ue_ipv4_addr,
                           uint32_t        gnb_v4_addr,
                           uint32_t        upf_v4_addr,
                           uint32_t        ul_teid,
                           uint32_t        dl_teid,
                           uint8_t         debug_flag);

#endif
