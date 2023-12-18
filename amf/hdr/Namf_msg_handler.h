#ifndef Namf_MSG_HANDLER_H
#define Namf_MSG_HANDLER_H

int
validate_rcvd_nmp_msg_on_Namf_interface(uint8_t *msg_ptr,
                                        int      msg_len);

int
send_service_registration_msg_to_nrf(uint8_t  debug_flag);

int
send_session_create_msg_to_smf(data_64bit_t  imsi,
                               uint8_t       debug_flag);

int
send_session_modify_msg_to_smf(data_64bit_t  imsi,
                               uint32_t gnb_n3_iface_v4_addr,
                               uint32_t dnlink_teid,
                               uint8_t  debug_flag);

extern uint32_t g__amf_ue_session_index;

#endif
