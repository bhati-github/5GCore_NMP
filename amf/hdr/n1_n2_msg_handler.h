#ifndef N1_N2_MSG_HANDLER_H
#define N1_N2_MSG_HANDLER_H

int
validate_rcvd_nmp_msg_on_n1_n2_interface(uint8_t *msg_ptr,
                                         int      msg_len);

int
send_all_ok_msg_to_gnodeb(nmp_msg_data_t *nmp_n1_n2_rcvd_msg_data_ptr,
                          uint8_t         debug_flag);

int
send_pdu_setup_failure_msg_to_gnodeb(nmp_msg_data_t *nmp_n1_n2_rcvd_msg_data_ptr,
                                     uint8_t         debug_flag);

int
process_rcvd_n1_n2_msg(nmp_msg_data_t *nmp_n1_n2_rcvd_msg_data_ptr,
                       uint32_t        gnb_n1_addr,
                       char           *msg_rcvd_time_string,
                       uint8_t         debug_flag);

int
listen_for_n1_n2_messages();

#endif
