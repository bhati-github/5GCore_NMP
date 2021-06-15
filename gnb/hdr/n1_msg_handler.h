#ifndef N1_MSG_HANDLER_H
#define N1_MSG_HANDLER_H

int
validate_rcvd_msg_on_n1_interface(uint8_t *msg_ptr,
                                  int      msg_len,
                                  uint32_t request_identifier);

int
listen_for_n1_messages();

#endif

