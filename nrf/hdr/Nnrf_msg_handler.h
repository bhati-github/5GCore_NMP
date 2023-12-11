#ifndef Nnrf_MSG_HANDLER_H
#define Nnrf_MSG_HANDLER_H

int
validate_rcvd_msg_on_Nnrf_interface(uint8_t *msg_ptr,
                                    int      msg_len,
                                    uint32_t request_identifier);

int
listen_for_Nnrf_messages();

#endif

