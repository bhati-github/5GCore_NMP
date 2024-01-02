#ifndef SESSION_MODIFY_H
#define SESSION_MODIFY_H

int
process_session_modify_request_msg(nmp_msg_data_t *nmp_n4_rcvd_msg_data_ptr,
                                   char           *msg_rcvd_time_string,
                                   uint8_t         debug_flag);

#endif
