#ifndef Nsmf_MSG_HANDLER_H
#define Nsmf_MSG_HANDLER_H

int
validate_rcvd_nmp_msg_on_Nsmf_interface(uint8_t *msg_ptr,
                                        int      msg_len);

int
send_service_registration_msg_to_nrf(uint8_t  debug_flag);

int
listen_for_Nsmf_messages();

#endif
