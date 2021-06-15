#ifndef N2_MSG_HANDLER_H
#define N2_MSG_HANDLER_H

int
validate_rcvd_msg_on_n2_interface(uint8_t *msg_ptr,
                                  int      msg_len,
                                  uint32_t request_identifier);

int
process_rcvd_n2_msg(uint8_t        *n2_msg_send_ptr,
                    nmp_msg_data_t *nmp_rcvd_n2_msg_data_ptr,
                    uint32_t        upf_n2_addr,
                    uint8_t         debug_flag);

#endif
