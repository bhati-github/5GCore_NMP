#ifndef SESSION_CREATE_H
#define SESSION_CREATE_H

int
process_session_create_request_msg(nmp_msg_data_t *nmp_n4_rcvd_msg_data_ptr,
                                   char           *msg_rcvd_time_string,
                                   uint8_t         debug_flag);

#endif
