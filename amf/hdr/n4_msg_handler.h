#ifndef N4_MSG_HANDLER_H
#define N4_MSG_HANDLER_H

int
validate_rcvd_msg_on_n4_interface(uint8_t *msg_ptr,
                                  int      msg_len,
                                  uint32_t request_identifier);

int
process_rcvd_n4_msg(uint8_t        *n4_msg_send_ptr,
                    nmp_msg_data_t *nmp_rcvd_n4_msg_data_ptr,
                    uint32_t        upf_n4_addr,
                    uint8_t         debug_flag);

#endif
