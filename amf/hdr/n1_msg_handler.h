#ifndef N1_MSG_HANDLER_H
#define N1_MSG_HANDLER_H

int
validate_rcvd_msg_on_n1_interface(uint8_t *msg_ptr,
                                  int      msg_len,
                                  uint32_t request_identifier);

int
process_rcvd_n1_msg(nmp_msg_data_t *nmp_n1_rcvd_msg_data_ptr,
                    uint32_t        enb_n1_addr,
                    uint8_t         debug_flag);

#endif
