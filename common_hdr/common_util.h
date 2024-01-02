#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

void
dump_bytes(char    *space,
           char    *str,
           uint8_t *ptr,
           int      len);

void
get_current_time(char *final_time);

void
get_ipv4_addr_string(uint32_t ipv4_addr, char *dst);

void
get_ipv6_addr_string(uint8_t *src, char *dst);

void
get_imsi_string(uint8_t *src, char *dst);

int
convert_user_imsi_to_pkt_data_format(char *str, int str_len, data_64bit_t *user_imsi);

#endif 
