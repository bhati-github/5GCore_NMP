#ifndef N2_MSG_HANDLER_H
#define N2_MSG_HANDLER_H

int
validate_rcvd_msg_on_n2_interface(uint8_t *msg_ptr,
                                  int      msg_len,
                                  uint32_t request_identifier);

int
listen_for_n2_messages();

#endif

